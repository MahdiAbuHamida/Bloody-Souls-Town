// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "ItemObject.h"
#include "BloodySoulsTown/InteractableActors/Items/Weapons/Weapon.h"
#include "WeaponItemObject.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API UWeaponItemObject final : public UItemObject
{
	GENERATED_BODY()

public:
	void Init(const FName InItemId, FIntPoint InDimensions, UMaterialInterface* InIcon,
		TSubclassOf<AItem> InItemClass, UMaterialInterface* InIconRotated,
		FString InItemName, FString InItemInfo, bool InIsGun, UTexture2D* InWeaponInWheelIcon, FVector2D InWeaponInWheelIconSize,
		TArray<FVector2D> InWeaponInWheelIconRenderPivot, const EWeaponType InWeaponType, const FWeaponData InWeaponData);

	
	UPROPERTY(BlueprintReadOnly)
	UTexture2D* WeaponInWheelIcon;
	
	UPROPERTY(BlueprintReadOnly)
	FVector2D WeaponInWheelIconSize = FVector2D(80, 40);
	
	UPROPERTY(BlueprintReadOnly)
	TArray<float> WeaponInWheelIconRenderAngle;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<FVector2D> WeaponInWheelIconRenderPivot;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EWeaponType> WeaponType;

	UPROPERTY(BlueprintReadOnly)
	FWeaponData WeaponData;

	UPROPERTY(BlueprintReadOnly)
	bool IsGun;

	bool IsUsedBefore = false;
	
	int32 GunObjectCurrentAmmo;

	int32 RightHandGunCurrentAmmo;
};