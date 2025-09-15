// Copyright Epic Games, Inc. All Rights Reserved.

#include "UEAutomationToolsEditor.h"

#include "AssetToolsModule.h"
#include "IPythonScriptPlugin.h"
#include "UnrealEdGlobals.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Developer/SmartDeveloperSettings.h"
#include "Editor/UnrealEdEngine.h"

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

static void PreFixAsset_Clicked()
{
	if (IPythonScriptPlugin::Get())
	{
		FString ScriptPath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Plugins/SmartEditorTools/Scripts/PreFixAllAsset.py"));
		
		FString PythonCommand = FString::Printf(TEXT("exec(open(r'%s').read())"), *ScriptPath);
		IPythonScriptPlugin::Get()->ExecPythonCommand(*PythonCommand);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PythonScriptPlugin is not enabled!"));
	}
}

static void RegisterGameEditorMenus()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuSection& Section = Menu->AddSection("PlayGameExtensions", TAttribute<FText>(), FToolMenuInsert("Play", EToolMenuInsertType::After));

	FToolMenuEntry Automation = FToolMenuEntry::InitToolBarButton(
	"PreFixAsset",
			FUIAction(
		FExecuteAction::CreateStatic(&PreFixAsset_Clicked),
		FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
		FIsActionChecked(),
		FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)),
	LOCTEXT("PreFixAssetButton", "PreFixAsset"),
	LOCTEXT("PreFixAssetDescription", "Run your Python script to add prefixes to assets"),
	FSlateIcon()
	);
	Automation.StyleNameOverride = "CalloutToolbar";
	Section.AddEntry(Automation);
}

void FUEAutomationToolsEditorModule::OnAssetPostRenamed(const TArray<FAssetRenameData>& Datas)
{
	
    if (Datas.IsEmpty())
    {
        return;
    }

    FAssetRenameData LastData = Datas.Last();
    UObject* Asset = LastData.Asset.Get();
    if (!Asset)
    {
        return;
    }

    USmartDeveloperSettings* SmartDeveloperSettings = GetMutableDefault<USmartDeveloperSettings>();
    if (!SmartDeveloperSettings)
    {
        return;
    }

    if (!SmartDeveloperSettings->bAutoPrefix || !SmartDeveloperSettings->AutoAssetPath.IsEmpty())
    {
        return;
    }
	
	FString PackageName = Asset->GetOutermost()->GetName();
	SmartDeveloperSettings->AutoAssetPath = PackageName;

	UE_LOG(LogTemp, Display, TEXT("OnAssetPostRenamed Start"));

	

    FTSTicker::GetCoreTicker().AddTicker(
	    FTickerDelegate::CreateLambda([](float DeltaTime)
	    {
	    	UE_LOG(LogTemp, Display, TEXT("PythonScript Start"));
			
		    if (IPythonScriptPlugin::Get())
		    {
			    FString ScriptPath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Plugins/SmartEditorTools/Scripts/AutoPreFix.py"));
			    FString PythonCommand = FString::Printf(TEXT("exec(open(r'%s').read())"), *ScriptPath);
			    IPythonScriptPlugin::Get()->ExecPythonCommand(*PythonCommand);
		    }
		    else
		    {
			    UE_LOG(LogTemp, Warning, TEXT("PythonScriptPlugin is not enabled!"));
		    }
   
	    	return false;
	    }),
	    0.f 
   );

}

void FUEAutomationToolsEditorModule::OnAssetRenamed(const FAssetData& AssetData, const FString& OldName)
{
	if (IPythonScriptPlugin::Get())
	{
		if (USmartDeveloperSettings* SmartDeveloperSettings = GetMutableDefault<USmartDeveloperSettings>())
		{
			SmartDeveloperSettings->AutoAssetPath = AssetData.GetSoftObjectPath().ToString();
		}
		FString ScriptPath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Plugins/SmartEditorTools/Scripts/AutoPreFix.py"));
		
		FString PythonCommand = FString::Printf(TEXT("exec(open(r'%s').read())"), *ScriptPath);
		IPythonScriptPlugin::Get()->ExecPythonCommand(*PythonCommand);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PythonScriptPlugin is not enabled!"));
	}
}

void FUEAutomationToolsEditorModule::StartupModule()
{
	if (!IsRunningGame())
	{
		if (FSlateApplication::IsInitialized())
		{
			ToolMenusHandle = UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&RegisterGameEditorMenus));
		}
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		RenameDelegateHandle = AssetToolsModule.Get().OnAssetPostRename().AddRaw(this,  &FUEAutomationToolsEditorModule::OnAssetPostRenamed);

		// FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		// RenameDelegateHandle = AssetRegistryModule.Get().OnAssetRenamed().AddRaw(this, &FUEAutomationToolsEditorModule::OnAssetRenamed);
		
	}
}

void FUEAutomationToolsEditorModule::ShutdownModule()
{
	if (UObjectInitialized() && ToolMenusHandle.IsValid())
	{
		UToolMenus::UnRegisterStartupCallback(ToolMenusHandle);
	}
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		FAssetToolsModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		AssetRegistryModule.Get().OnAssetPostRename().Remove(RenameDelegateHandle);
	}
	
	// if (FModuleManager::Get().IsModuleLoaded("AssetRegistry"))
	// {
	// 	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	// 	AssetRegistryModule.Get().OnAssetRenamed().Remove(RenameDelegateHandle);
	// }
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUEAutomationToolsEditorModule, UEAutomationTools)