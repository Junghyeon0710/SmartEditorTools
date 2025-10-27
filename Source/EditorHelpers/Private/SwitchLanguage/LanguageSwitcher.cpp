// Fill out your copyright notice in the Description page of Project Settings.


#include "SwitchLanguage/LanguageSwitcher.h"

#include "InternationalizationSettingsModel.h"
#include "SCulturePicker.h"
#include "SCulturePicker.h"
#include "Internationalization/Culture.h"
#include "Kismet/KismetInternationalizationLibrary.h"

static void SwitchLanguageButtonClicked(FString Language)
{
	if (GConfig)
	{
		const TArray<FString> LocalizedCultureNames = FTextLocalizationManager::Get().GetLocalizedCultureNames(ELocalizationLoadFlags::Editor);
		UInternationalizationSettingsModel* SettingsModel = NewObject<UInternationalizationSettingsModel>();
		bool bExist = LocalizedCultureNames.Contains(Language);
		if (bExist)
		{
			if (SettingsModel)
			{
				SettingsModel->SetEditorLanguage(Language);
			}

			FInternationalization& I18N = FInternationalization::Get();
			I18N.SetCurrentLanguage(Language);
			for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
			{
				UClass* CurrentClass = *ClassIt;

				if (UEdGraphSchema* Schema = Cast<UEdGraphSchema>(CurrentClass->GetDefaultObject()))
				{
					Schema->ForceVisualizationCacheClear();
				}
			}
		}
	}
}

TSharedRef<SWidget> FLanguageSwitcher::GetLanguagesDropdown()
{
	FMenuBuilder MenuBuilder(true, nullptr);
	
	TArray< TSharedPtr<FCultureEntry> > StockEntries;

	TArray<FString> StockCultureNames;
	FInternationalization::Get().GetCultureNames(StockCultureNames);

	TMap<FString, TSharedPtr<FCultureEntry>> TopLevelStockCultureEntries;
	TMap<FString, TSharedPtr<FCultureEntry>> AllStockCultureEntries;
	AllStockCultureEntries.Reserve(StockCultureNames.Num());

	for (const FString& CultureName : StockCultureNames)
	{
		const FCulturePtr Culture = FInternationalization::Get().GetCulture(CultureName);
		if (Culture.IsValid())
		{
			TArray<FString> HierarchicalCultureNames = Culture->GetPrioritizedParentCultureNames();
			if (HierarchicalCultureNames.Num() == 0 || HierarchicalCultureNames[0] != CultureName)
			{
				HierarchicalCultureNames.Remove(CultureName);
				HierarchicalCultureNames.Insert(CultureName, 0);
			}

			// Walk the array backwards to process the cultures in parent->child order
			TSharedPtr<FCultureEntry> ParentCultureEntry;
			const int32 TopLevelCultureIndex = HierarchicalCultureNames.Num() - 1;
			for (int32 CultureIndex = TopLevelCultureIndex; CultureIndex >= 0; --CultureIndex)
			{
				// Find the culture data
				const FString HierarchicalCultureName = HierarchicalCultureNames[CultureIndex];
				const FCulturePtr HierarchicalCulture = FInternationalization::Get().GetCulture(HierarchicalCultureName);
				if (!HierarchicalCulture.IsValid())
				{
					continue;
				}

				// Find or add a map entry for this culture
				TSharedPtr<FCultureEntry>& StockCultureEntryRef = AllStockCultureEntries.FindOrAdd(HierarchicalCultureName);
				if (!StockCultureEntryRef.IsValid())
				{
					StockCultureEntryRef = MakeShareable(new FCultureEntry(HierarchicalCulture));

					// Link this entry as a child of its parent
					if (ParentCultureEntry.IsValid())
					{
						ParentCultureEntry->Children.Add(StockCultureEntryRef);
					}
				}

				// Is this culture a top-level entry?
				if (CultureIndex == TopLevelCultureIndex)
				{
					TSharedPtr<FCultureEntry>& TopLevelStockCultureEntryRef = TopLevelStockCultureEntries.FindOrAdd(HierarchicalCultureName);
					if (!TopLevelStockCultureEntryRef.IsValid())
					{
						TopLevelStockCultureEntryRef = StockCultureEntryRef;
					}
				}

				ParentCultureEntry = StockCultureEntryRef;
			}
		}
	}

	// Populate the top-level array
	StockEntries.Reserve(TopLevelStockCultureEntries.Num());
	for (const auto& CultureNameDataPair : TopLevelStockCultureEntries)
	{
		StockEntries.Add(CultureNameDataPair.Value);
	}

	// Sort entries
	StockEntries.Sort([](const TSharedPtr<FCultureEntry>& LHS, const TSharedPtr<FCultureEntry>& RHS) -> bool
	{
		const FString LHSDisplayName = GetCultureDisplayName(LHS->Culture.ToSharedRef(),SCulturePicker::ECultureDisplayFormat::ActiveCultureDisplayName, false);
		const FString RHSDisplayName = GetCultureDisplayName(RHS->Culture.ToSharedRef(),SCulturePicker::ECultureDisplayFormat::ActiveCultureDisplayName, false);
		return FTextComparison::CompareTo(LHSDisplayName, RHSDisplayName) < 0;
	});

	TArray<FString> AllowedCultures = {
		TEXT("ko"),        
		TEXT("en"),        
		TEXT("ja"),        
		TEXT("zh-Hans"),   
	};

	for (const FString& CultureName : AllowedCultures)
	{
		const FCulturePtr Culture = FInternationalization::Get().GetCulture(CultureName);
		if (!Culture.IsValid())
			continue;

		MenuBuilder.AddMenuEntry(
			FText::FromString(Culture->GetDisplayName()),
			FText::FromString(Culture->GetNativeName()), 
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateStatic(&SwitchLanguageButtonClicked, Culture->GetName()))
		);
	}

	return MenuBuilder.MakeWidget();

}

void FLanguageSwitcher::SwitchLanguageKeyboardButtonClicked()
{
	if (GConfig) {

		const TArray<FString> LocalizedCultureNames = FTextLocalizationManager::Get().GetLocalizedCultureNames(ELocalizationLoadFlags::Editor);

		FString SourceLangStr = "en";
		FString TargetLangStr = "ko";

		FString CurrentLanguage = UKismetInternationalizationLibrary::GetCurrentLanguage();
		FString NewLanguage = (CurrentLanguage == SourceLangStr) ? TargetLangStr : SourceLangStr;
		UInternationalizationSettingsModel* SettingsModel = NewObject<UInternationalizationSettingsModel>();
		bool bExist = LocalizedCultureNames.Contains(NewLanguage);
		if (bExist) {
			SettingsModel->SetEditorLanguage(NewLanguage);
			// if (not SettingsModel->ShouldUseLocalizedPropertyNames()) {
			// 	SettingsModel->SetShouldUseLocalizedPropertyNames(bExist);
			// }
			// if (not SettingsModel->ShouldUseLocalizedNodeAndPinNames()) {
			// 	SettingsModel->SetShouldUseLocalizedNodeAndPinNames(bExist);
			// }
			FInternationalization& I18N = FInternationalization::Get();
			I18N.SetCurrentLanguage(NewLanguage);
			// Find all Schemas and force a visualization cache clear
			for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
			{
				UClass* CurrentClass = *ClassIt;

				if (UEdGraphSchema* Schema = Cast<UEdGraphSchema>(CurrentClass->GetDefaultObject()))
				{
					Schema->ForceVisualizationCacheClear();
				}
			}
		}


	}
}

FString FLanguageSwitcher::GetCultureDisplayName(const FCultureRef& Culture,SCulturePicker::ECultureDisplayFormat DisplayNameFormat, const bool bIsRootItem)
{
	const FString DisplayName = Culture->GetDisplayName();
	if (DisplayNameFormat == SCulturePicker::ECultureDisplayFormat::ActiveCultureDisplayName)
	{
		return DisplayName;
	}

	const FString NativeName = Culture->GetNativeName();
	if (DisplayNameFormat == SCulturePicker::ECultureDisplayFormat::NativeCultureDisplayName)
	{
		return NativeName;
	}

	if (DisplayNameFormat == SCulturePicker::ECultureDisplayFormat::ActiveAndNativeCultureDisplayName)
	{
		// Only show both names if they're different (to avoid repetition), and we're a root item (to avoid noise)
		return (bIsRootItem && !NativeName.Equals(DisplayName, ESearchCase::CaseSensitive))
			? FString::Printf(TEXT("%s [%s]"), *DisplayName, *NativeName)
			: DisplayName;
	}

	if (DisplayNameFormat == SCulturePicker::ECultureDisplayFormat::NativeAndActiveCultureDisplayName)
	{
		// Only show both names if they're different (to avoid repetition), and we're a root item (to avoid noise)
		return (bIsRootItem && !NativeName.Equals(DisplayName, ESearchCase::CaseSensitive))
			? FString::Printf(TEXT("%s [%s]"), *NativeName, *DisplayName)
			: NativeName;
	}

	return DisplayName;
}
