// Mahdi Abu Hamida - Final University Project (2022)


#include "QuestItem.h"

#include "BloodySoulsTown/InteractableActors/Items/ItemObjects/QuestItemObject.h"

/* Getters */

AQuestItem::AQuestItem()
{
	this->ItemType = Item_Quest;

	if (this->StaticMeshComponent != nullptr)
	{
		this->StaticMeshComponent->bRenderCustomDepth = false;
	}
}

int32 AQuestItem::GetQuestId() const
{
	return this->QuestId;
}

EQuestType AQuestItem::GetQuestType() const
{
	return QuestType;
}

AQuestItemsCraftingTable* AQuestItem::GetUsedTable() const
{
	return UsedTable;
}

void AQuestItem::SetTable(const AQuestItemsCraftingTable* InTable)
{
	UsedTable = const_cast<AQuestItemsCraftingTable*>(InTable);
}

UItemObject* AQuestItem::GetDefaultItemObject()
{
	UQuestItemObject* QuestItemObject = NewObject<UQuestItemObject>(this);
	if (QuestItemObject != nullptr)
	{
		QuestItemObject->Init(this->ItemId, this->Dimensions, this->Icon, this->ItemClass,
			this->IconRotated, this->ItemName, this->ItemInfo, this->QuestId, this->QuestType);
	}
	
	this->ItemObject = QuestItemObject;
	return this->ItemObject;
}
