// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/InteractableActors/Interactable.h"
#include "BloodySoulsTown/InteractableActors/QuestItemsCraftingTable/Enumurations/QuestItemsCraftingTableType.h"
#include "QuestItemsCraftingTable.generated.h"

struct FQuestClass_Name;
/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API AQuestItemsCraftingTable final : public AInteractable
{
	GENERATED_BODY()

public:
	AQuestItemsCraftingTable();

	void UseTable(const FQuestClass_Name& InQuestClass_Name);

	void ReleaseUsingTable();

	
	/* Getters */

	class UBoxComponent* GetBoxCollisionComponent() const;
	
	EQuestItemsCraftingTableType GetQuestItemsCraftingTableType() const;

	bool GetIsInUse() const;

	
	/* Setters */
	
	void SetCanUseTable(const bool InbCanUse);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float CraftItemTime = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	FVector SpawnQuestItemRelativeLocation;
	
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	FRotator SpawnQuestItemRelativeRotation;

private:
	UFUNCTION()
	void FinishBuildingItem(const TSubclassOf<class AQuestItem> InQuestItemClass);

	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* BoxCollisionComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	TEnumAsByte<EQuestItemsCraftingTableType> QuestItemsCraftingTableType;

	UPROPERTY(EditDefaultsOnly, Category = "User Interface")
	TSubclassOf<UUserWidget> CraftingTableWidgetClass;

	UPROPERTY()
	class UQuestItemsCraftingTableWidget* CraftingTableWidget;
	
	
	FTimerHandle TimerHandle_FinishCraftingItem;
	
	bool bInUse;

	bool bCanUse;
};
