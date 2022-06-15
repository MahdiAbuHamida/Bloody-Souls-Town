// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/InteractableActors/Items/Item.h"
#include "Enumurations/QuestType.h"
#include "QuestItem.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API AQuestItem final : public AItem
{
	GENERATED_BODY()

public:

	AQuestItem();

	
	/* Getters */

	// Method returns the quest id
	UFUNCTION(BlueprintCallable)
	int32 GetQuestId() const;

	// Method returns the quest type
	EQuestType GetQuestType() const;

	class AQuestItemsCraftingTable* GetUsedTable() const;


	/* Setters */
	
	void SetTable(const class AQuestItemsCraftingTable* InTable);
	
protected:
	virtual UItemObject* GetDefaultItemObject() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Quest Properties")
	TEnumAsByte<EQuestType> QuestType;

	UPROPERTY(EditDefaultsOnly, Category = "Quest Properties")
	int32 QuestId = -1;

private:
	UPROPERTY()
	AQuestItemsCraftingTable* UsedTable;
};