// Mahdi Abu Hamida - Final University Project (2022)


#include "AmmoItemObject.h"

void UAmmoItemObject::Init(const FName InItemId, FIntPoint InDimensions, UMaterialInterface* InIcon,
	TSubclassOf<AItem> InItemClass, UMaterialInterface* InIconRotated, FString InItemName, FString InItemInfo,
	const EWeaponType InAmmoTypeByWeapon, const int32 InAmmoAmountToAdd, const int32 InMaxAmmoAmountPerItem
	, const int32 InMaxAmmoForType)
{
	Super::Init(InItemId, InDimensions, InIcon, InItemClass, InIconRotated, InItemName, InItemInfo);

	this->ItemType = Item_Ammo;
	this->AmmoTypeByWeapon = InAmmoTypeByWeapon;
	this->AmmoAmountToAdd = InAmmoAmountToAdd;
	this->MaxAmmoAmountPerItem = InMaxAmmoAmountPerItem;
	this->MaxAmmoForType = InMaxAmmoForType;
}

void UAmmoItemObject::Copy(UAmmoItemObject* AmmoItemObject)
{
	FString Id = this->ItemId.ToString();
	Id.Append("Copied");
	Id.Append(FString::FromInt(this->CopiesCounter));
	++this->CopiesCounter;
	
	AmmoItemObject->ItemId = FName(*Id);
	AmmoItemObject->ItemClass = this->ItemClass;
	AmmoItemObject->ItemName = this->ItemName;
	AmmoItemObject->ItemInfo = this->ItemInfo;
	AmmoItemObject->Dimensions = this->Dimensions;
	AmmoItemObject->Icon = this->Icon;
	AmmoItemObject->IconRotated = this->IconRotated;

	AmmoItemObject->ItemType = Item_Ammo;
	AmmoItemObject->AmmoTypeByWeapon = this->AmmoTypeByWeapon;
	AmmoItemObject->AmmoAmountToAdd = this->AmmoAmountToAdd;
	AmmoItemObject->MaxAmmoAmountPerItem = this->MaxAmmoAmountPerItem;
	AmmoItemObject->MaxAmmoForType = this->MaxAmmoForType;
}
