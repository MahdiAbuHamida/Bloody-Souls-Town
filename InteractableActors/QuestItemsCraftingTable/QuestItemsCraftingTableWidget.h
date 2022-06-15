// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "QuestItemsCraftingTableWidget.generated.h"

/**
 * 
 */
UCLASS( Abstract )
class BLOODYSOULSTOWN_API UQuestItemsCraftingTableWidget final : public UUserWidget
{
	GENERATED_BODY()

public:
	void CustomInit(FTimerManager* InTimerManager, const FString InQuestItemName,
		const EQuestItemsCraftingTableType InQuestItemsCraftingTableType, const float InCraftItemTime)
	{
		TimerManager = InTimerManager;
		QuestItemName = InQuestItemName;
		QuestItemsCraftingTableType = InQuestItemsCraftingTableType;
		CraftItemTime = InCraftItemTime;
	}

	void StopProgressTimer()
	{
		if (TimerManager != nullptr)
		{
			TimerManager->ClearTimer(TimerHandle_TickProgressBar);
		}
	}
	
protected:
	explicit UQuestItemsCraftingTableWidget(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer), QuestItemName("Quest Item Name"){}
	
	virtual void NativeConstruct() override
	{
		CraftingProgressBar->SetPercent(0.0f);
		ProgressBar_SizeBox->SetWidthOverride(CraftItemTime * 80.0f);
		
		FString Str("Crafting The ");
		Str.Append(QuestItemName);
		CraftingText->SetText(FText::FromString(Str));

		if (TimerManager != nullptr)
		{
			TimerManager->SetTimer(TimerHandle_TickProgressBar, this,
				&UQuestItemsCraftingTableWidget::IncreaseProgressBar, 0.01f, true);	
		}
	}
	
	virtual void NativePreConstruct() override
	{
		CraftingText->SetText(FText::FromString("Crafting The "));
	}

private:
	void IncreaseProgressBar() const
	{
		// CraftItemTime = x, TimerRate = y, PercentAddition = z, MaximumPercentage = 1
		// z = y / x
		CraftingProgressBar->SetPercent(CraftingProgressBar->Percent + 0.01f / CraftItemTime);
	}

protected:
	UPROPERTY(meta = (BindWidget))
	USizeBox* ProgressBar_SizeBox;
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* CraftingProgressBar;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* CraftingText;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EQuestItemsCraftingTableType> QuestItemsCraftingTableType;
	
	FString QuestItemName;

	float CraftItemTime;
	
	FTimerManager* TimerManager;
	
	FTimerHandle TimerHandle_TickProgressBar;
};
