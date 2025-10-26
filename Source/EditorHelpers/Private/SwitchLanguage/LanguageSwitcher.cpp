// Fill out your copyright notice in the Description page of Project Settings.


#include "SwitchLanguage/LanguageSwitcher.h"

#include "Internationalization/Culture.h"

static void SwitchLanguageButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("SwitchLanguageButtonClicked"));
}

TSharedRef<SWidget> FLanguageSwitcher::GetLanguagesDropdown()
{
	FMenuBuilder MenuBuilder(true, nullptr);
	
	TArray<FString> CultureNames;
	FInternationalization::Get().GetCultureNames(CultureNames);
	
	for (const FString& CultureName : CultureNames)
	{
		const FCulturePtr Culture = FInternationalization::Get().GetCulture(CultureName);
		if (!Culture.IsValid())
		{
			continue;
		}

		const FText DisplayName = FText::FromString(Culture->GetDisplayName());

		MenuBuilder.AddMenuEntry(
			DisplayName,
			FText::FromString(Culture->GetNativeName()),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateStatic(&SwitchLanguageButtonClicked)
			)
		);
	}

	return MenuBuilder.MakeWidget();
}
