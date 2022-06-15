// M&M Games 2021 - Free University Project


#include "QuestItemObject.h"

void UQuestItemObject::Init(const FName InItemId, FIntPoint InDimensions, UMaterialInterface* InIcon,
		TSubclassOf<AItem> InItemClass, UMaterialInterface* InIconRotated,
		FString InItemName, FString InItemInfo, const int32 InQuestId, const EQuestType InQuestType)
{
	Super::Init(InItemId, InDimensions, InIcon, InItemClass, InIconRotated, InItemName, InItemInfo);
	
	this->QuestId = InQuestId;
	this->ItemType = Item_Quest;
	this->QuestType = InQuestType;
}

TEnumAsByte<EQuestType> UQuestItemObject::GetQuestType() const
{
	return this->QuestType;
}

int32 UQuestItemObject::GetQuestId() const
{
	return this->QuestId;
}
