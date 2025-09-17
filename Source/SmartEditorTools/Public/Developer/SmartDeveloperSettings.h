// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SmartDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Editor, DefaultConfig, meta=(DisplayName="SmartEditor Tools"), MinimalAPI)
class USmartDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
#if WITH_EDITORONLY_DATA
public:
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category=Maps, meta=(AllowedClasses="/Script/Engine.World"))
	TArray<FSoftObjectPath> EditorMaps;

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category=Assets, meta=(ContentDir))
	TArray<FDirectoryPath> PreFixAssetPaths;

	UPROPERTY(BlueprintReadWrite)
	FString AutoAssetPath = {};

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category=Assets)
	bool bAutoPrefix = false;

#endif
	
protected:
	
	virtual FName GetCategoryName() const override
	{
		return TEXT("Plugins");
	}
};
