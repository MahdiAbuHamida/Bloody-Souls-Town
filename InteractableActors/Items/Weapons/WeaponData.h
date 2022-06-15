// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "WeaponData.generated.h"

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

	FORCEINLINE FWeaponData(){}
	
	FORCEINLINE FWeaponData(const float InDamage, const float InRange, const float InFireRate,
		const float InAmmoCapacity, const float InMagCapacity)
	: Damage(InDamage), Range(InRange), FireRate(InFireRate), AmmoCapacity(InAmmoCapacity), MagCapacity(InMagCapacity){}
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Damage = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Range = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float FireRate = 0.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 AmmoCapacity = 0;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 MagCapacity = 0;
};
