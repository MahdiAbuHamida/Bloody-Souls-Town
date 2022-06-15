// Mahdi Abu Hamida - Final University Project (2022)


#include "BloodySoulsTown/PlayerComponents/QuestHandler/QuestHandler.h"

#include "BloodySoulsTown/InteractableActors/Items/ItemObjects/QuestItemObject.h"
#include "BloodySoulsTown/InteractableActors/Items/QuestItem/Enumurations/QuestType.h"
#include "BloodySoulsTown/InteractableActors/Items/QuestItem/Structs/QuestData.h"
#include "BloodySoulsTown/PlayerComponents/Inventory/QuestInventory/QuestInventory.h"

// Sets default values for this component's properties
UQuestHandler::UQuestHandler()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UQuestHandler::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() != nullptr)
	{
		QuestInventoryComp = Cast<UQuestInventory>(GetOwner()->GetComponentByClass(UQuestInventory::StaticClass()));
	}
	
}

FQuestClass_Name UQuestHandler::HandleShieldQuest() const
{
	FQuestClass_Name QuestClass_ItemName = FQuestClass_Name();
	if (this->QuestInventoryComp == nullptr || this->QuestInventoryComp->GetQuestItemsByType(
		EQuestType::ShieldPart).Num() != ShieldQuestStruct.PartsRequired)
	{
		return QuestClass_ItemName;
	}

	if (QuestInventoryComp->GetQuestItemsByType(EQuestType::Shield).Num())
	{
		return QuestClass_ItemName;
	}
	
	if (QuestClass_Name_StructsPerClass.Contains(ShieldItemClass))
	{
		QuestClass_ItemName.QuestItemClass = ShieldItemClass;
		QuestClass_ItemName.QuestItemName = QuestClass_Name_StructsPerClass[ShieldItemClass].QuestItemName;
	}
	return QuestClass_ItemName;
}

FQuestClass_Name UQuestHandler::HandleKeyQuest()
{
	FQuestClass_Name QuestClass_Name = FQuestClass_Name();
	if (QuestInventoryComp == nullptr) return QuestClass_Name;

	TArray<UQuestItemObject*> KeyQuestItemObjects = QuestInventoryComp->GetQuestItemsByType(EQuestType::KeyPart);
	const TArray<FQuestData> CompletedQuests = QuestInventoryComp->GetCompletedQuests();
	for (const FQuestData QuestStruct : QuestStructsCollection)
	{
		int32 PartsFound = 0;
		for (UQuestItemObject* KeyQuestItemObject : KeyQuestItemObjects)
		{
			if (KeyQuestItemObject != nullptr && KeyQuestItemObject->GetQuestId() == QuestStruct.QuestId)
			{
				++PartsFound;
				if (PartsFound == QuestStruct.PartsRequired)
				{
					QuestInventoryComp->AddCompletedQuest(QuestStruct);
					if (KeyItemsClassesPerQuestId.Contains(QuestStruct.QuestId))
					{
						const TSubclassOf<AQuestItem>& Class = KeyItemsClassesPerQuestId[QuestStruct.QuestId];
						if (QuestClass_Name_StructsPerClass.Contains(Class))
						{
							QuestClass_Name.QuestItemClass = Class;
							QuestClass_Name.QuestItemName = QuestClass_Name_StructsPerClass[Class].QuestItemName;
						}
					}
					return QuestClass_Name;
				}
			}
		}
	}
	
	return QuestClass_Name;
}

FQuestData& UQuestHandler::GetShieldQuestStruct()
{
	return ShieldQuestStruct;
}

