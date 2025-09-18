// Copyright Epic Games, Inc. All Rights Reserved.

#include "EditorHelpers.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "UnrealEdGlobals.h"
#include "Developer/SmartDeveloperSettings.h"
#include "Editor/UnrealEdEngine.h"
#include "IPluginBrowser.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/AssetRegistryInterface.h"
#include "Runtime/AssetRegistry/Private/PackageDependencyData.h"


#define LOCTEXT_NAMESPACE "FUEAutomationToolsModule"

static bool HasPlayWorld()
{
	return GEditor->PlayWorld != nullptr;
}

static bool HasNoPlayWorld()
{
	return !HasPlayWorld();
}

static bool HasPlayWorldAndRunning()
{
	return HasPlayWorld() && !GUnrealEd->PlayWorld->bDebugPauseExecution;
}

static void OpenCommonMap_Clicked(FString MapPath)
{
	if (ensure(MapPath.Len()))
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(MapPath);
	}
}

static bool CanShowCommonMaps()
{
	return HasNoPlayWorld() && !GetDefault<USmartDeveloperSettings>()->EditorMaps.IsEmpty();
}

static TSharedRef<SWidget> GetCommonMapsDropdown()
{
	FMenuBuilder MenuBuilder(true, nullptr);
	
	for (const FSoftObjectPath& Path : GetDefault<USmartDeveloperSettings>()->EditorMaps)
	{
		if (!Path.IsValid())
		{
			continue;
		}
		
		const FText DisplayName = FText::FromString(Path.GetAssetName());
		MenuBuilder.AddMenuEntry(
			DisplayName,
			LOCTEXT("CommonPathDescription", "Opens this map in the editor"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateStatic(&OpenCommonMap_Clicked, Path.ToString()),
				FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
				FIsActionChecked(),
				FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)
			)
		);
	}

	return MenuBuilder.MakeWidget();
}

static void RegisterGameEditorMenus()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuSection& Section = Menu->AddSection("PlayGameExtensions", TAttribute<FText>(), FToolMenuInsert("Play", EToolMenuInsertType::After));

	FToolMenuEntry CommonMapEntry = FToolMenuEntry::InitComboButton(
		"CommonMapOptions",
		FUIAction(
			FExecuteAction(),
			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateStatic(&CanShowCommonMaps)),
		FOnGetContent::CreateStatic(&GetCommonMapsDropdown),
		LOCTEXT("CommonMaps_Label", "Common Maps"),
		LOCTEXT("CommonMaps_ToolTip", "Some commonly desired maps while using the editor"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Level")
	);
	CommonMapEntry.StyleNameOverride = "CalloutToolbar";
	Section.AddEntry(CommonMapEntry);
}

void FEditorHelpersModule::StartupModule()
{
	if (!IsRunningGame())
	{
		if (FSlateApplication::IsInitialized())
		{
			ToolMenusHandle = UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&RegisterGameEditorMenus));
		}
	}
	InitCBMenuExtension();
}

void FEditorHelpersModule::ShutdownModule()
{
	if (UObjectInitialized() && ToolMenusHandle.IsValid())
	{
		UToolMenus::UnRegisterStartupCallback(ToolMenusHandle);
	}
}

void FEditorHelpersModule::InitCBMenuExtension()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	TArray<FContentBrowserMenuExtender_SelectedPaths>& ContentBrowserModuleMenuExtenders = ContentBrowserModule.GetAllPathViewContextMenuExtenders();
	
	ContentBrowserModuleMenuExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FEditorHelpersModule::CustomCBMenuExtender));
}

TSharedRef<FExtender> FEditorHelpersModule::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender(new FExtender());

	if (SelectedPaths.Num() > 0)
	{
		MenuExtender->AddMenuExtension(
			FName("Delete"),
			EExtensionHook::After,
			nullptr,
			FMenuExtensionDelegate::CreateRaw(this, &FEditorHelpersModule::AddCBMenuEntry)
			);

		FolderPathsSelected = SelectedPaths;
	}
	
	return MenuExtender;
}

void FEditorHelpersModule::AddCBMenuEntry(class FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		LOCTEXT("Delete Unused Asset Label", "Delete Unused Asset"),
		LOCTEXT("Delete Unused Asset ToolTip",  "Safely delete all unused assets under folder"),
		FSlateIcon(FAppStyle::Get().GetStyleSetName(), "Icons.Delete"),
		FExecuteAction::CreateRaw(this, &FEditorHelpersModule::OnDeleteUnusedAssetButtonClicked)
		);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("Delete Empty Folders Label", "Delete Empty Folders"),
		LOCTEXT("Delete Empty Folders ToolTip",  "Safely delete all empty folders"),
		FSlateIcon(FAppStyle::Get().GetStyleSetName(), "Icons.Delete"),
		FExecuteAction::CreateRaw(this, &FEditorHelpersModule::OnDeleteEmptyFoldersButtonClikced)
		);
	
}

void FEditorHelpersModule::OnDeleteUnusedAssetButtonClicked()
{
	if (FolderPathsSelected.Num() > 1)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("UnusedAssetTwoButtonClickedMessage","You can only do this to one folder"));
		return;
	}

	TArray<FString> AssetPathNames = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);

	if (AssetPathNames.Num() == 0)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("UnusedAssetNoAssetsMessage", "No assets found in folder"));
		return;
	}

	
	FText DialogText = FText::Format(LOCTEXT("UnusedAssetMessage", "{0} assets need to be checked.\nWould you like to proceed?"),FText::AsNumber(AssetPathNames.Num()));

	EAppReturnType::Type ConfirmResult = FMessageDialog::Open(EAppMsgType::YesNo, DialogText);

	if (ConfirmResult == EAppReturnType::No)
	{
		return;
	}
	
	FixUpRedirectors();
	
	TArray<FAssetData> UnusedAssetsDataArray;

	for (const FString& AssetPathName : AssetPathNames)
	{
		if (AssetPathName.Contains(TEXT("Developers")) || AssetPathName.Contains(TEXT("Collections")) ||
			AssetPathName.Contains(TEXT("__ExternalActors__")) ||
			AssetPathName.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))
		{
			continue;
		}


		if (GetDefault<USmartDeveloperSettings>()->bDependsOnOtherAssets)
		{
			FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
			IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		
			TArray<FName> Dependencies;
			if (AssetRegistry.GetDependencies(*FPackageName::ObjectPathToPackageName(AssetPathName), Dependencies, UE::AssetRegistry::EDependencyCategory::Package))
			{
				int32 NumDependencies = Dependencies.Num();

				for (auto& Dependency : Dependencies)
				{
					if (!Dependency.ToString().StartsWith("/Game") && !Dependency.ToString().StartsWith("/Plugins"))
					{
						NumDependencies--;
					}
				}

				if (NumDependencies > 0)
				{
					continue;
				}
			}
		}
		
		TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName);
		if (AssetReferencers.Num() == 0)
		{
			const FAssetData UnusedAssetData = UEditorAssetLibrary::FindAssetData(AssetPathName);
			UnusedAssetsDataArray.Add(UnusedAssetData);
		}
	}

	if (UnusedAssetsDataArray.Num() > 0)
	{
		ObjectTools::DeleteAssets(UnusedAssetsDataArray);
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("UnusedAssetNoAssetsMessage", "No unused assets found"));
	}

}

void FEditorHelpersModule::OnDeleteEmptyFoldersButtonClikced()
{
	FixUpRedirectors();

	TArray<FString> FolderPathsArray = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0],true,true);
	uint32 Counter = 0;

	FString EmptyFolderPathsNames;
	TArray<FString> EmptyFoldersPathsArray;

	for(const FString& FolderPath:FolderPathsArray)
	{
		if(FolderPath.Contains(TEXT("Developers"))||
		FolderPath.Contains(TEXT("Collections")) ||
		FolderPath.Contains(TEXT("__ExternalActors__")) ||
		FolderPath.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}

		if(!UEditorAssetLibrary::DoesDirectoryExist(FolderPath)) continue;

		if(!UEditorAssetLibrary::DoesDirectoryHaveAssets(FolderPath))
		{
			EmptyFolderPathsNames.Append(FolderPath);
			EmptyFolderPathsNames.Append(TEXT("\n"));

			EmptyFoldersPathsArray.Add(FolderPath);
		}		
	}

	if(EmptyFoldersPathsArray.Num()==0)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("", "No empty folder found under selected folder"));
		return;
	}

	EAppReturnType::Type ConfirmResult = FMessageDialog::Open(EAppMsgType::OkCancel, FText::FromString(TEXT("Empty folders found in:\n") + EmptyFolderPathsNames + TEXT("\nWould you like to delete all?")));

	if(ConfirmResult==EAppReturnType::Cancel) return;
	
	for(const FString& EmptyFolderPath:EmptyFoldersPathsArray)
	{
		UEditorAssetLibrary::DeleteDirectory(EmptyFolderPath) ? ++Counter :
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Failed to delete ") + EmptyFolderPath));
	}

	if(Counter>0)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Successfully deleted ") + FString::FromInt(Counter) + TEXT(" folders")));
	}
}

void FEditorHelpersModule::FixUpRedirectors()
{
	TArray<UObjectRedirector*> RedirectorsToFixArray;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace("/Game");
	Filter.PackagePaths.Emplace(TEXT("/Game"));

	TArray<FAssetData> Redirectors;
	AssetRegistryModule.Get().GetAssets(Filter, Redirectors);
	
	TArray<UObject*> RedirectorObjects;
	for (const FAssetData& Asset : Redirectors)
	{
		if (UObjectRedirector* Redirector = Cast<UObjectRedirector>(Asset.GetAsset()))
		{
			RedirectorObjects.Add(Redirector);
		}
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray);
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEditorHelpersModule, UEAutomationTools)