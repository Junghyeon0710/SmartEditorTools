// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "EditorStyleSet.h"
#include "UObject/Object.h"

/**
 * 
 */

class FLanguageSwitchCommands : public TCommands<FLanguageSwitchCommands>
{
public:

	FLanguageSwitchCommands()
		: TCommands<FLanguageSwitchCommands>(TEXT("LanguageSwitch"), NSLOCTEXT("Contexts", "LanguageSwitch", "LanguageSwitch Plugin"), NAME_None, FEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
