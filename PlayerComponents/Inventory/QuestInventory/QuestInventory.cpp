// Mahdi Abu Hamida - Final University Project (2022)


#include "BloodySoulsTown/PlayerComponents/Inventory/QuestInventory/QuestInventory.h"

#include "QuestInventoryWidget.h"
#include "BloodySoulsTown/InteractableActors/Items/ItemObjects/QuestItemObject.h"
#include "Blueprint/UserWidget.h"

// Sets default values for this component's properties
UQuestInventory::UQuestInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
}


/* Getters */

TArray<UQuestItemObject*> UQuestInventory::GetQuestItems() const
{
	return this->QuestItems;
}

TArray<UQuestItemObject*> UQuestInventory::GetQuestItemsByType(const EQuestType InType) const
{
	TArray<UQuestItemObject*> Array;
	for (UQuestItemObject* QuestItemObject : this->QuestItems)
	{
		if (QuestItemObject != nullptr && QuestItemObject->GetQuestType() == InType)
		{
			Array.AddUnique(QuestItemObject);
		}
	}
	return Array;
}

TArray<FQuestData> UQuestInventory::GetCompletedQuests()
{
	return this->CompletedQuests;
}

void UQuestInventory::AddCompletedQuest(const FQuestData InQuestData)
{
	this->CompletedQuests.AddUnique(InQuestData);
}

void UQuestInventory::RemoveItem(UItemObject* InQuestItem)
{
	if (UQuestItemObject* QuestItemObject = Cast<UQuestItemObject>(InQuestItem))
	{
		if (QuestItems.Remove(QuestItemObject))
		{
			QuestInventoryWidget->Refresh();
			if (GEngine != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Cyan,
					FString::Printf(TEXT("Quest item removed from the quest inventory!")));
			}
		}
	}
}


/* Setters */

void UQuestInventory::SetQuestInventoryWidget(UQuestInventoryWidget* InQuestInventoryWidget)
{
	this->QuestInventoryWidget = InQuestInventoryWidget;
}

// Called when the game starts
void UQuestInventory::BeginPlay()
{
	Super::BeginPlay();
}

void UQuestInventory::AddQuestItem(UItemObject* InQuestItem)
{
	if (UQuestItemObject* QuestItemObject = Cast<UQuestItemObject>(InQuestItem))
	{
		this->QuestItems.AddUnique(QuestItemObject);

		if (this->QuestInventoryWidget != nullptr)
		{
			this->QuestInventoryWidget->Refresh();
		}
	}
}

