// Mahdi Abu Hamida - Final University Project (2022)


#include "WeaponItemObject.h"

void UWeaponItemObject::Init(const FName InItemId, FIntPoint InDimensions, UMaterialInterface* InIcon,
	TSubclassOf<AItem> InItemClass, UMaterialInterface* InIconRotated, FString InItemName, FString InItemInfo,
	bool InIsGun, UTexture2D* InWeaponInWheelIcon, FVector2D InWeaponInWheelIconSize,
	TArray<FVector2D> InWeaponInWheelIconRenderPivot, const EWeaponType InWeaponType, const FWeaponData InWeaponData)
{
	Super::Init(InItemId, InDimensions, InIcon, InItemClass, InIconRotated, InItemName, InItemInfo);
	this->IsGun = InIsGun;
	this->WeaponInWheelIcon = InWeaponInWheelIcon;
	this->WeaponInWheelIconSize = InWeaponInWheelIconSize;
	this->WeaponInWheelIconRenderPivot = InWeaponInWheelIconRenderPivot;

	// Top left index in the weapons wheel
	this->WeaponInWheelIconRenderAngle.Add(-45.f);
	// top right index
	this->WeaponInWheelIconRenderAngle.Add(45.f);
	// bottom right index
	this->WeaponInWheelIconRenderAngle.Add(135.f);
	// bottom left index
	this->WeaponInWheelIconRenderAngle.Add(-135.0);

	this->ItemType = Item_Weapon;
	this->WeaponType = InWeaponType;
	this->WeaponData = InWeaponData;
}