// M&M Games 2021 - Free University Project


#include "AmmoItem.h"

#include "BloodySoulsTown/InteractableActors/Items/ItemObjects/AmmoItemObject.h"

void AAmmoItem::SetAmmoItemAmmoAmount(const int32 InAmmoAmount)
{
	this->AmmoAmount = InAmmoAmount;
	if (UAmmoItemObject* AmmoItemObject = Cast<UAmmoItemObject>(this->ItemObject))
	{
		AmmoItemObject->AmmoAmountToAdd = InAmmoAmount;
	}
}

void AAmmoItem::SetAmmoType(const EWeaponType InAmmoType)
{
	this->AmmoType = InAmmoType;
}

UItemObject* AAmmoItem::GetDefaultItemObject()
{
	this->ItemType = Item_Ammo;
	UAmmoItemObject* AmmoItemObject = NewObject<UAmmoItemObject>(this);
	AmmoItemObject->Init(this->ItemId, this->Dimensions, this->Icon, this->ItemClass,
		this->IconRotated, this->ItemName, this->ItemInfo, this->AmmoType, this->AmmoAmount,
		this->MaxAmmoPerItem, this->MaxAmmoForType);

	this->ItemObject = AmmoItemObject;
	return this->ItemObject;
}
