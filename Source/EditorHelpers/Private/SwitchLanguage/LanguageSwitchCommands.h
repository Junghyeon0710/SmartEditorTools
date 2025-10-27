// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LanguageSwitchCommands.generated.h"

/**
 * 
 */

class FLanguageSwitchCommands : public TCommands<FLanguageSwitchCommands>
{
public:

	FLanguageSwitchCommands()
		: TCommands<FLanguageSwitchCommands>(TEXT("LanguageSwitch"), NSLOCTEXT("Contexts", "LanguageSwitch", "LanguageSwitch Plugin"), NAME_None, NAME_None)
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
