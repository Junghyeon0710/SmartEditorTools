// Fill out your copyright notice in the Description page of Project Settings.


#include "LanguageSwitchCommands.h"

#define LOCTEXT_NAMESPACE "FLanguageSwitchCommands"

void FLanguageSwitchCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Language", "Execute Language action", EUserInterfaceActionType::Button, FInputChord(EKeys::L));
}

#undef LOCTEXT_NAMESPACE