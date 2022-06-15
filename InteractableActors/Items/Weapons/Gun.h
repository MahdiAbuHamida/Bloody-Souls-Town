// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/InteractableActors/Items/Weapons/Weapon.h"
#include "Gun.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangeDel, int32, CurrentAmmo, int32, CurrentTotalAmmo);

UCLASS()
class BLOODYSOULSTOWN_API AGun final : public AWeapon
{
	GENERATED_BODY()

public:
	AGun();

	/*
	  Called to initialize the gun's ammo amount from the player's inventory, related to the type of ammo.
	  in case of dual guns, the left hand gun calls this method only after the right hand gun calls it
	 */
	void InitializeAmmo();
	
	// Called to check if gun is in a ready state to fire
	virtual bool PrepareAttack() override;
	
	// Called to attack (fire shot - pull trigger)
	virtual void Attack() override;
	
	// Called to stop attacking (release trigger)
	virtual void StopAttack() override;

	// Called to reload the gun
	UFUNCTION(BlueprintCallable)
	void ReloadGun();
	

	/* Getters */

	// Method returns the gun skeletal mesh component
	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetSkeletalMeshComponent() const;

	// Method returns whether the gun has ammo or not (in magazine)
	bool GetHasAmmo() const;

	// Method returns whether the gun's magazine is full or not
	bool GetIsMagFull();

	// Method returns the reload time for the gun
	float GetReloadTime() const;

	// Method returns the gun's reloading animation
	UAnimationAsset* GetGunReloadAnimation() const;

	// Method returns the available ammo amount for reload
	int32 GetAmmoAmountAvailableForReload();

	// Method returns the gun's current ammo (in magazine)
	UFUNCTION(BlueprintPure)
	int32 GetCurrentAmmo() const;

	// Method returns the total gun's ammo (in inventory)
	UFUNCTION(BlueprintPure)
	int32 GetCurrentTotalAmmo() const;

	// Method returns gun's bullet spread amount
	float GetBulletSpread() const;

	
	/* Setters */

	// Method to set the bullet spread amount
	UFUNCTION(BlueprintCallable)
	void SetBulletSpread(const float InBulletSpread);

	// Method to set total ammo of a gun
	void SetCurrentTotalAmmo(const int32 InNewTotalAmmo);
	
	UPROPERTY(BlueprintAssignable)
	FOnAmmoChangeDel OnAmmoChangeDel;

	
	/* Gun abilities */

	// Max ammo amount in gun's magazine
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon Abilities")
	int32 MaxMagAmmo = 24;

	
	/* Values used for modifying player bones in the anim graph */
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Anim Graph")
	float ZOffset = 5.f;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Anim Graph")
	float YOffset = 0.f;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Anim Graph")
	float LeftShoulderGunAdsZOffset = 0.f;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Anim Graph")
	float LeftShoulderZRotOffset = 1.2f;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Anim Graph")
	float RightShoulderZRotOffset = 0.f;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void AutomaticRecoil();

	// Called to calculate a hit result before firing a projectile bullet/ explosive shot
	void CalculateShot(class UCameraComponent* OwnerCamera, AController* OwnerController, const uint16 InBulletsCounter);

	// Called to spawn a projectile
	void ProjectileEffect(FHitResult Hit, bool bSuccess, UCameraComponent* OwnerCamera, FVector ShotDirection,
		const FVector TrailEndLocation, AController* OwnerController, const uint16 InBulletsCounter);

	/*
	  Method returns whether there's an extra ammo in the player's
	  inventory for this type of gun or not
	 */
	bool HasExtraAmmo() const;
	

	/* Gun components */
	
	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* SkeletalMeshComponent;

	
	/* Gun sockets */

	// Gun bullet shell socket
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Sockets")
	FName GunShellSocket = "";

	// Gun muzzle flash socket
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Sockets")
	FName GunMuzzleFlashSocket = "";

	
	/* Gun animations */

	// Gun reload animation
	UPROPERTY(EditDefaultsOnly, Category = "Gun Animations")
	UAnimationAsset* ReloadAnimation;

	
	/* Gun abilities */
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Abilities")
	float ReloadTime = 0.7f;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon Abilities")
	float BulletSpread = 0.f;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon Abilities")
	float AdsBulletSpread = 0.f;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon Abilities")
	float RecoilIntensity = 30.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Abilities")
	int32 BulletsPerShot = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Abilities")
	float GunLoudness = 1.f;

	
	/* Gun effects*/
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Effects")
	UParticleSystem* GunShellParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Effects")
	UParticleSystem* TrailFx;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Effects")
	TSubclassOf<class AProjectileBase> WeaponProjectileClass;

	// Camera shake after pull trigger
	UPROPERTY(EditAnywhere, Category = "Camera Shakes")
	TSubclassOf<UMatineeCameraShake> CameraShakeClass;
	
	int32 CurrentAmmo = 0;
	
	int32 CurrentTotalAmmo = 0;
	
	bool IsMagFull;

	UPROPERTY()
	class AAlexCharacter* OwnerCharacter = nullptr;
	
	UPROPERTY()
	class UInventory* PlayerInventory = nullptr;

private:
	// Called to load the gun from save slot
	void LoadGun();
};