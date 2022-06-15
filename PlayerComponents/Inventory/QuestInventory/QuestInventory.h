// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/InteractableActors/Items/QuestItem/Structs/QuestData.h"
#include "Components/ActorComponent.h"
#include "QuestInventory.generated.h"

class UQuestItemObject;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class BLOODYSOULSTOWN_API UQuestInventory final : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UQuestInventory();

	void AddCompletedQuest(const FQuestData InQuestData);

	void RemoveItem(class UItemObject* InQuestItem);

	
	/* Getters */
	
	// Method returns all quest items in the inventory
	UFUNCTION(BlueprintPure)
	TArray<UQuestItemObject*> GetQuestItems() const;

	TArray<UQuestItemObject*> GetQuestItemsByType(const enum EQuestType InType) const;

	TArray<FQuestData> GetCompletedQuests();

	
	/* Setters */
	
	UFUNCTION(BlueprintCallable)
	void SetQuestInventoryWidget(class UQuestInventoryWidget* InQuestInventoryWidget);

	void AddQuestItem(UItemObject* InQuestItem);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	UQuestInventoryWidget* QuestInventoryWidget;
	
	TArray<FQuestData> CompletedQuests;
	
private:
	TArray<UQuestItemObject*> QuestItems;
};
