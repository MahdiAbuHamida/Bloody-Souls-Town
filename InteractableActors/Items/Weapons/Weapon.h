// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "WeaponData.h"
#include "BloodySoulsTown/InteractableActors/Items/Item.h"
#include "WeaponType.h"
#include "Weapon.generated.h"

UCLASS()
class BLOODYSOULSTOWN_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	
	/* Getters */

	// Method returns the box collision component of a mystery box weapon
	class UBoxComponent* GetBoxComponent() const;

	// Method returns delay (seconds) between each attack
	float GetDelayBetweenAttacks() const;

	// Method returns player character skeletal mesh "weapon" socket name
	FName GetPlayerWeaponSocketName() const;

	// Method returns player character skeletal mesh "left weapon" socket name
	FName GetPlayerWeaponSocketNameLeftHand() const;

	// Method returns the weapon type
	TEnumAsByte<EWeaponType> GetWeaponType() const;

	// Method returns whether the weapon is explosive or not
	bool GetIsExplosiveWeapon() const;

	// Method returns the impulse amount weapon applies on kill hit
	float GetWeaponKillImpulse() const;

	// Method returns the weapon attack range
	float GetWeaponAttackRange() const;

	// Method returns the weapon damage amount
	float GetWeaponDamageAmount() const;


	/* Setters */

	// Method to set the delay between a rapid attacks (when player gets rapid fire power stone)
	void SetDelayBetweenRapidAttacks();


	/* Main methods */
	
	// Called to check if weapon is in ready state to attack
	virtual bool PrepareAttack();

	// Called to attack
	virtual void Attack();

	// Called to stop attack
	virtual void StopAttack();

	// Called to stop attack with left hand weapon
	void StopAttackLeft();

	UPROPERTY(EditDefaultsOnly)
	FWeaponData WeaponData;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Booleans")
	bool IsAutomaticWeapon = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Booleans")
	bool IsDualWeapons = false;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Abilities")
	int32 MaxAmmoForGun = 100;

	bool IsLeftGun = false;

	FText SavedActionText;
	
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float PlayerNormalWalkingSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float PlayerMaximumWalkingSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float PlayerWalkingSpeedAds = 180.f;

	UPROPERTY()
	class AMysteryBoxBase* MysteryBoxRef;

protected:
	virtual void BeginPlay() override;

	// Initializes an Item object for the weapon
	virtual UItemObject* GetDefaultItemObject() override;

	UPROPERTY()
	UBoxComponent* BoxCollision;

	// Blueprint events functions
	UFUNCTION(BlueprintImplementableEvent, Category = "Animation Blueprint")
	void OnRecoilStartsEvent();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Sockets")
	FName PlayerWeaponSocketName = "WeaponDefault_Socket";

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Sockets")
	FName PlayerWeaponSocketNameLeftHand = "WeaponDefault_Socket";

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon Properties")
	TEnumAsByte<EWeaponType> WeaponType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Animations")
	UAnimationAsset* AttackAnimation;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "User Interface")
	UTexture2D* WeaponInViewPortImage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "User Interface")
	FVector2D WeaponImageInViewPortSize = FVector2D(640, 360);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "User Interface")
	UTexture2D* WeaponCrossHairImage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "User Interface")
	FVector2D WeaponCrossHairSize = FVector2D(7, 7);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "User Interface")
	bool IsDynamicCrossHair = true;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "User Interface")
	FVector2D DynamicCrossHairScale = FVector2D(100.f, 100.f);

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Abilities")
	float Range = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Abilities")
	float DamageAmount = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Abilities")
	float DelayBetweenAttacks = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Abilities")
	float DelayBetweenRapidAttacks;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Abilities")
	float KillImpulse = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Booleans")
	bool IsGun = true;

	UPROPERTY(EditDefaultsOnly, Category = "Booleans")
	bool IsExplosive = false;

	UPROPERTY(EditAnywhere, Category = "Item Object Properties|Weapons Wheel")
	UTexture2D* WeaponInWheelImage;

	UPROPERTY(EditAnywhere, Category = "Item Object Properties|Weapons Wheel")
	FVector2D WeaponInWheelImageSize = FVector2D(40, 80);

	UPROPERTY(EditAnywhere, Category = "Item Object Properties|Weapons Wheel")
	TArray<FVector2D> RenderPivot;

	FTimerHandle StopAttackTimerHandle;

	int32 AttacksCounter = 0;
};
