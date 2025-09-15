// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

struct  FAssetRenameData;

class FUEAutomationToolsEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void OnAssetRenamed(const FAssetData& AssetData, const FString& OldName);
	void OnAssetPostRenamed(const TArray<FAssetRenameData>& Data);
	void OnAssetAdded(const FAssetData& Path);


	bool bAssetRenamed = false;
private:
	FDelegateHandle ToolMenusHandle;
	FDelegateHandle RenameDelegateHandle;
	FDelegateHandle RenamePostDelegateHandle;
	FDelegateHandle AddDelegateHandle;

};
