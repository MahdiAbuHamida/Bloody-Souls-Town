// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "ItemObject.h"
#include "BloodySoulsTown/InteractableActors/Items/Weapons/WeaponType.h"
#include "AmmoItemObject.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API UAmmoItemObject final : public UItemObject
{
	GENERATED_BODY()

public:
	void Init(const FName InItemId, FIntPoint InDimensions, UMaterialInterface* InIcon,
		TSubclassOf<AItem> InItemClass, UMaterialInterface* InIconRotated,
		FString InItemName, FString InItemInfo, const EWeaponType InAmmoTypeByWeapon, const int32 InAmmoAmountToAdd,
		const int32 InMaxAmmoAmountPerItem, const int32 InMaxAmmoForType);

	// Copy custom constructor method
	void Copy(UAmmoItemObject* AmmoItemObject);

	// Ammo type
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EWeaponType> AmmoTypeByWeapon;

	// Max ammo amount for each 1 ammo item (in the inventory)
	UPROPERTY(BlueprintReadOnly)
	int32 MaxAmmoAmountPerItem;

	// Max ammo overall
	UPROPERTY(BlueprintReadOnly)
	int32 MaxAmmoForType;
	
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentTotalAmmo = 0;
	
	int32 AmmoAmountToAdd = 0;

	int32 CopiesCounter = 0;
};