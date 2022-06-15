// M&M Games 2021 - Free University Project

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/InteractableActors/Items/Item.h"
#include "BloodySoulsTown/InteractableActors/Items/Weapons/WeaponType.h"
#include "AmmoItem.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API AAmmoItem final : public AItem
{
	GENERATED_BODY()

public:
	/*** Setters ***/
	
	void SetAmmoItemAmmoAmount(const int32 InAmmoAmount);

	void SetAmmoType(const EWeaponType InAmmoType);
	
protected:
	// Initializes an Item object for the Ammo Item
	virtual UItemObject* GetDefaultItemObject() override;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EWeaponType> AmmoType;

	UPROPERTY(EditAnywhere)
	int32 AmmoAmount = 10;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxAmmoPerItem = 30;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxAmmoForType = 100;
};