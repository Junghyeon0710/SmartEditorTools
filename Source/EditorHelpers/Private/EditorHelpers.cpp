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
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FEditorHelpersModule::OnDeleteUnusedAssetButtonClicked)
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

	
	FText DialogText = FText::Format(LOCTEXT("UnusedAssetMessage", "{0} assets found.\nWould you like to proceed?"),FText::AsNumber(AssetPathNames.Num()));

	EAppReturnType::Type ConfirmResult = FMessageDialog::Open(EAppMsgType::YesNo, DialogText);

	if (ConfirmResult == EAppReturnType::No)
	{
		return;
	}
	
	FixUpRedirectors();
	
	TArray<FAssetData> UnusedAssetsDataArray;

	for (const FString& AssetPathName : AssetPathNames)
	{
		if (AssetPathName.Contains(TEXT("Developers")) || AssetPathName.Contains(TEXT("Collections")))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))
		{
			continue;
		}

		
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