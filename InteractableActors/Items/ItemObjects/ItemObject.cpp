// Mahdi Abu Hamida - Final University Project (2022)


#include "ItemObject.h"

// Initialize the item object properties
void UItemObject::Init(const FName InItemId, const FIntPoint InDimensions, UMaterialInterface* InIcon, const TSubclassOf<AItem> InItemClass,
					UMaterialInterface* InIconRotated, const FString InItemName, const FString InItemInfo)
{
	this->ItemId = InItemId;
	this->ItemClass = InItemClass;
	this->ItemName = InItemName;
	this->ItemInfo = InItemInfo;
	this->Dimensions = InDimensions;
	this->Icon = InIcon;
	this->IconRotated = InIconRotated;
}


/* Getters */

// Method returns the item object id
FName UItemObject::GetItemId() const
{
	return this->ItemId;
}

// Method returns the dimensions of the item object (icon dimensions)
FIntPoint UItemObject::GetDimensions() const
{
	return this->bIsIconRotated ? FIntPoint(this->Dimensions.Y, this->Dimensions.X) : this->Dimensions;
}

// Method returns the icon of the item object
UMaterialInterface* UItemObject::GetIcon() const
{
	return this->bIsIconRotated ? this->IconRotated : this->Icon;
}

// Method returns the item class
TSubclassOf<AItem> UItemObject::GetItemClass() const
{
	return this->ItemClass;
}

// Method returns whether the item object icon is rotated or not
bool UItemObject::GetIsIconRotated() const
{
	return this->bIsIconRotated;
}

// Method returns the item object name
FString UItemObject::GetItemName() const
{
	return this->ItemName;
}

// Method returns info about the item
FString UItemObject::GetItemInfo() const
{
	return this->ItemInfo;
}

// Method returns the item type
EItemType UItemObject::GetItemType() const
{
	return this->ItemType;
}

/*
  Method that rotates the Item object
  used to check if there's a space in the player's inventory,
  after regular iterating failed, by rotating the item object,
  rotating is just replacing between x (width) and y (height) values.
 */
void UItemObject::RotateIcon()
{
	this->bIsIconRotated = !this->bIsIconRotated;
}


/* Setters */

// Method to set the item object id
void UItemObject::SetItemId(const FName InItemId)
{
	this->ItemId = InItemId;
}

// Method to set the item index
void UItemObject::SetItemIndex(const int32 InIndex)
{
	this->ItemIndex = InIndex;
}
