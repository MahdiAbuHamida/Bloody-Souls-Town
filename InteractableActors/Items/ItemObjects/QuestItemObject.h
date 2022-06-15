// M&M Games 2021 - Free University Project

#pragma once

#include "CoreMinimal.h"
#include "ItemObject.h"
#include "BloodySoulsTown/InteractableActors/Items/QuestItem/Enumurations/QuestType.h"
#include "QuestItemObject.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API UQuestItemObject final : public UItemObject
{
	GENERATED_BODY()

public:
	void Init(const FName InItemId, FIntPoint InDimensions, UMaterialInterface* InIcon,
		TSubclassOf<AItem> InItemClass, UMaterialInterface* InIconRotated,
		FString InItemName, FString InItemInfo, const int32 InQuestId, const EQuestType InQuestType);

	UFUNCTION(BlueprintPure)
	TEnumAsByte<EQuestType> GetQuestType() const;

	UFUNCTION(BlueprintPure)
	int32 GetQuestId() const;

private:
	int32 QuestId;
	EQuestType QuestType;
};