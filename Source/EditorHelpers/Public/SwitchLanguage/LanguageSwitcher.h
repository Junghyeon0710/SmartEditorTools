// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SCulturePicker.h"


class FLanguageSwitcher 
{
public:
	
	static TSharedRef<SWidget> GetLanguagesDropdown();

	static FString GetCultureDisplayName(const FCultureRef& Culture, SCulturePicker::ECultureDisplayFormat DisplayNameFormat, const bool bIsRootItem);
};
