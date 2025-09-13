// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AutoDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Editor, DefaultConfig, meta=(DisplayName="Automation Tools"), MinimalAPI)
class UAutoDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category=Maps, meta=(AllowedClasses="/Script/Engine.World"))
	TArray<FSoftObjectPath> EditorMaps;

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category=Assets, meta=(ContentDir))
	TArray<FDirectoryPath> PreFixAssetPaths;

protected:
	virtual FName GetCategoryName() const override
	{
		return TEXT("Plugins");
	}

	
};
