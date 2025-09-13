// Copyright Epic Games, Inc. All Rights Reserved.

#include "UEAutomationToolsEditor.h"

#include "UnrealEdGlobals.h"
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
	UE_LOG(LogTemp, Warning, TEXT("PreFixAsset_Clicked"));
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

void FUEAutomationToolsEditorModule::StartupModule()
{
	if (!IsRunningGame())
	{
		if (FSlateApplication::IsInitialized())
		{
			ToolMenusHandle = UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&RegisterGameEditorMenus));
		}
	}
}

void FUEAutomationToolsEditorModule::ShutdownModule()
{
	if (UObjectInitialized() && ToolMenusHandle.IsValid())
	{
		UToolMenus::UnRegisterStartupCallback(ToolMenusHandle);
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUEAutomationToolsEditorModule, UEAutomationTools)