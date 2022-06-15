// Mahdi Abu Hamida - Final University Project (2022)


#include "Gun.h"

#include "ProjectileBase.h"
#include "BloodySoulsTown/Characters/AlexCharacter.h"
#include "BloodySoulsTown/PlayerComponents/Inventory/Inventory.h"
#include "BloodySoulsTown/Characters/AI/EnemyCharacter.h"
#include "BloodySoulsTown/Other/MainGameState.h"
#include "BloodySoulsTown/Other/MainPlayerState.h"
#include "BloodySoulsTown/Other/MySaveGame.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"


AGun::AGun()
{
	this->SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	this->SkeletalMeshComponent->SetupAttachment(this->SceneRoot);

	// Outline effect
	this->StaticMeshComponent->SetRenderCustomDepth(false);
	this->SkeletalMeshComponent->SetRenderCustomDepth(this->bUseRenderCustomDepth);

	// Default trail fx particle system asset
	ConstructorHelpers::FObjectFinder<UParticleSystem>const TrailFxParticleSystem(TEXT("ParticleSystem'/Game/Resources/Effects/ParticleSystems/Weapons/AssaultRifle/Muzzle/P_AssaultRifle_Trail.P_AssaultRifle_Trail'"));
	this->TrailFx = TrailFxParticleSystem.Object;
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

	/*
	  This class inherits from interactable actor, since it should have Static Mesh,
	  this actual class (Gun), uses the Skeletal Mesh Component instead, so the static mesh gets destroyed
	  Skeletal Mesh for guns: it gives the ability to play animations on the gun's mesh
	  and can take advantage of knowing where different parts of the gun's are located
	  to play the animations and other particle effect etc..
	 */
	if (!this->IsDualWeapons)
	{
		this->StaticMeshComponent->RemoveFromRoot();
		this->StaticMeshComponent->DestroyComponent();
	}
	
	this->OwnerCharacter = Cast<AAlexCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (this->OwnerCharacter != nullptr)
	{
		this->PlayerInventory = this->OwnerCharacter->GetPlayerInventory();
		// Check if need to set fire rate to be rapid fire (in case the player owns rapid fire power stone)
		if (this->OwnerCharacter->GetPlayerPowerStonesByTypes().Contains(PowerStone_RapidFire))
		{
			this->SetDelayBetweenRapidAttacks();
		}
	}
	// Save game has been cancelled
	// this->LoadGun();
}

/*
  Called to initialize the gun's ammo amount from the player's inventory, related to the type of ammo.
  in case of dual guns, the left hand gun calls this method only after the right hand gun calls it
 */
void AGun::InitializeAmmo()
{
	const int32 FinalCurrentAmmo = FMath::Max(this->CurrentAmmo, 0);
	/*
	  Check if the gun used before and a current ammo value still exists (let's say in the mag),
	  if true then this gun's current ammo (in the magazine) will be restored.
	 */
	UWeaponItemObject* WeaponItemObject = Cast<UWeaponItemObject>(this->GetItemObject());
	if (WeaponItemObject != nullptr && this->IsDualWeapons && !this->IsLeftGun)
	{
		if (WeaponItemObject->RightHandGunCurrentAmmo > 0)
		{
			this->CurrentAmmo = WeaponItemObject->RightHandGunCurrentAmmo;
		}
		else
		{
			this->CurrentAmmo = FinalCurrentAmmo;
		}
	}
	else if (WeaponItemObject != nullptr && WeaponItemObject->GunObjectCurrentAmmo > 0
			&& (this->IsLeftGun || !this->IsDualWeapons))
	{
		this->CurrentAmmo = WeaponItemObject->GunObjectCurrentAmmo;
	}
	else
	{
		this->CurrentAmmo = FinalCurrentAmmo;
	}
	

	// Trying to add initial ammo for the gun, only if it's being used for the first time by the player
	if (WeaponItemObject != nullptr)
	{
		// If the gun not being used before, then try to add more initial ammo
		if (!WeaponItemObject->IsUsedBefore)
		{
			// The current ammo can't go beyond the max mag ammo
			this->CurrentAmmo = FMath::Min(this->CurrentAmmo + this->MaxMagAmmo, this->MaxMagAmmo);
			// Try to set is used before to true
			if (this->IsDualWeapons && this->IsLeftGun)
			{
				WeaponItemObject->IsUsedBefore = true;
			}
			else if (!this->IsDualWeapons)
			{
				WeaponItemObject->IsUsedBefore = true;
			}
		}
	}

	// Try getting total ammo from the player's inventory
	if (this->PlayerInventory != nullptr)
	{
		this->CurrentTotalAmmo = this->PlayerInventory->GetAmmoByWeaponType(this->WeaponType);
	}
	else
	{
		// Default value
		this->CurrentTotalAmmo = 0;
	}
	
	// If there's ammo in total and current ammo (in the mag) = 0, then force the player for a reload
	if (this->HasExtraAmmo() && this->CurrentAmmo == 0 && this->OwnerCharacter != nullptr)
	{
		if (this->IsDualWeapons)
		{
			if (this->IsLeftGun)
			{
				this->OwnerCharacter->Reload();
			}
		}
		else
		{
			this->OwnerCharacter->Reload();
		}
		// Update time between attacks
		if (this->OwnerCharacter->GetPlayerPowerStonesByTypes().Contains(PowerStone_RapidFire))
		{
			this->DelayBetweenAttacks = this->DelayBetweenRapidAttacks;
		}
	}
	
	// Notify that the ammo amounts might has been changed, update widget UI texts
	this->OnAmmoChangeDel.Broadcast(this->CurrentAmmo, this->CurrentTotalAmmo);
}

// Called to check if gun is in a ready state to fire
bool AGun::PrepareAttack()
{
	Super::PrepareAttack();
	
	if (this->GetHasAmmo())
	{
		return true;
	}
	if (this->HasExtraAmmo() && this->OwnerCharacter != nullptr)
	{
		this->OwnerCharacter->Reload();
	}
	return false;
}

// Called to attack (fire shot - pull trigger)
void AGun::Attack()
{
	Super::Attack();
	
	if (this->OwnerCharacter == nullptr) return;
	
	AMainPlayerState* PlayerState = Cast<AMainPlayerState>(this->OwnerCharacter->GetPlayerState());
	if (PlayerState == nullptr) return;
	
	// Check if the gun has ammo in magazine
	if (this->GetHasAmmo())
	{
		// Player controller used for the projectile when causing damage
		AController* OwnerController = this->OwnerCharacter->GetController();
		if (OwnerController == nullptr) return;
		// Player camera used to determine the test line trace to get some initial values before spawning the projectile
		UCameraComponent* OwnerCamera = this->OwnerCharacter->GetPlayerCamera();
		if (OwnerCamera == nullptr) return;

		// Decrease ammo by 1
		--this->CurrentAmmo;
		
		UWeaponItemObject* WeaponItemObject = Cast<UWeaponItemObject>(this->GetItemObject());
		// Update weapon item object ammo amounts
		if (WeaponItemObject != nullptr)
		{
			if (this->IsDualWeapons && !this->IsLeftGun)
			{
				WeaponItemObject->RightHandGunCurrentAmmo = this->CurrentAmmo;
			}
			else
			{
				WeaponItemObject->GunObjectCurrentAmmo = this->CurrentAmmo;
			}
		}
		// Try to play gun fire animation if exists
		if (this->AttackAnimation != nullptr)
		{
			this->SkeletalMeshComponent->PlayAnimation(this->AttackAnimation, false);
			// Play camera shake
			if (IsValid(this->CameraShakeClass))
			{
				UGameplayStatics::PlayWorldCameraShake(this->GetWorld(), this->CameraShakeClass, this->GetActorLocation(),
					0.f, 500.f, 1.f, false);
			}
			// Notify to make a noise call so AI can hear the event of a gun fire
			OwnerCharacter->ReportNoise(this->GetActorLocation(), this->GunLoudness);
		}

		/*
		  If player is aiming do not show bullet shell particle
		  otherwise show it (if shell socket name & particle is valid)
		 */
		const bool bShowBulletShell = !this->OwnerCharacter->bIsAiming &&
			this->GunShellParticle != nullptr && this->GunShellSocket != "";
		
		if (bShowBulletShell)
		{
			const FTransform ShellSocketTransform = this->SkeletalMeshComponent->GetSocketTransform(this->GunShellSocket);
			const FVector ShellSocketLocation = ShellSocketTransform.GetLocation();
			const FQuat ShellSocketRotation = ShellSocketTransform.GetRotation();
			// Spawn gun bullet shell particle
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), this->GunShellParticle, ShellSocketLocation,
				ShellSocketRotation.Rotator());
		}

		// Some guns fires more than one like the shotguns
		uint16 BulletsCounter = 0;
		// Means we need to iterate until number of shots ends
		while (BulletsCounter < this->BulletsPerShot)
		{
			// Increase player shots fired (for accuracy calculation)
			PlayerState->IncreaseShotsFired();
			CalculateShot(OwnerCamera, OwnerController, BulletsCounter+1);
			++BulletsCounter;
		}
		++this->AttacksCounter;
		return;
	}
	// There's no ammo in gun's magazine
	if (this->IsLeftGun)
	{
		// Stop is attacking (left hand gun)
		this->OwnerCharacter->SetLeftHandIsPlayerAttacking(false);
	}
	else
	{
		// Stop is attacking
		this->OwnerCharacter->SetIsPlayerAttacking(false);
	}
	// Check if player has ammo in inventory for a reload
	if (this->HasExtraAmmo())
	{
		// Player reload the gun
		this->OwnerCharacter->Reload();
	}
}

// Called to calculate a hit result before firing a projectile bullet/ explosive shot
void AGun::CalculateShot(UCameraComponent* OwnerCamera, AController* OwnerController, const uint16 InBulletsCounter)
{
	// Line trace setup
	
	FHitResult Hit;
	// Player camera transform (line trace starts from player camera location)
	FTransform CameraTransform = OwnerCamera->GetComponentTransform();
	FVector StartLocation = CameraTransform.GetLocation();
	FVector EndLocation = StartLocation + CameraTransform.Rotator().Vector() * this->Range;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	// Firing the line trace
	bool bSuccess =  UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), StartLocation, EndLocation, ObjectTypes,
		false, ActorsToIgnore, EDrawDebugTrace::None, OUT Hit, true);
	const FVector TrailEndLocation = bSuccess ? Hit.ImpactPoint : EndLocation;
	// Projectile effect
	if (IsValid(this->WeaponProjectileClass))
	{
		FVector ShotDirection = -CameraTransform.Rotator().Vector();
		this->ProjectileEffect(Hit, bSuccess, OwnerCamera, ShotDirection, TrailEndLocation, OwnerController, InBulletsCounter);
	}
	// Stop attack if gun is not automatic
	if (!this->IsAutomaticWeapon)
	{
		if (this->OwnerCharacter != nullptr)
		{
			this->OwnerCharacter->StopAttack();
		}
	}
	else
	{
		this->AutomaticRecoil();
	}
	// Ammo change update to widgets
	this->OnAmmoChangeDel.Broadcast(this->CurrentAmmo, this->CurrentTotalAmmo);
}

// Called to spawn a projectile
void AGun::ProjectileEffect(FHitResult Hit, bool bSuccess, UCameraComponent* OwnerCamera, FVector ShotDirection,
	const FVector TrailEndLocation, AController* OwnerController, const uint16 InBulletsCounter)
{
	float TempDamage = this->DamageAmount;
	// If hit succeeded
	if (bSuccess)
	{
		// Damage from gun according to physics body part (e.g. head=100%, chest=50%, etc)
		TempDamage *= Hit.PhysMaterial->DestructibleDamageThresholdScale;
	}
	// If an actor was hit
	if (Hit.GetActor() != nullptr)
	{
		// Higher range = more stable damage, distance to actor hit / range of the gun 
		const float Distance = this->GetDistanceTo(Hit.GetActor());
		TempDamage -= Distance*2/this->Range;
		// In case the damage went very low (normally doesn't happen)
		TempDamage = FMath::Max(TempDamage, 5.f);
	}
	// Gun muzzle flash socked location
	FVector MuzzleFlashSocketLocation = this->GetSkeletalMeshComponent()->GetSocketLocation(this->GunMuzzleFlashSocket);
	// If hit succeeded
	if (bSuccess)
	{
		// Rotation from muzzle flash towards the impact point (end location - impact)
		const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(MuzzleFlashSocketLocation, Hit.ImpactPoint);
		const FTransform SpawnTransform(Rotation, MuzzleFlashSocketLocation, FVector(1, 1, 1));
		// Spawn projectile
		AProjectileBase* ProjectileToSpawn = Cast<AProjectileBase>(
			UGameplayStatics::BeginDeferredActorSpawnFromClass(this, this->WeaponProjectileClass,
				SpawnTransform));
		// Initialize projectile properties before running the construction script
		ProjectileToSpawn->Init(OwnerController, ShotDirection, this, TempDamage, InBulletsCounter);
		UGameplayStatics::FinishSpawningActor(ProjectileToSpawn, SpawnTransform);
	}
	// Otherwise, get rotation of player pawn
	else
	{
		APawn* OwnerPawn = Cast<APawn>(OwnerCamera->GetOwner());
		if (OwnerPawn != nullptr)
		{
			const FRotator Rotation = OwnerPawn->GetControlRotation();
			const FTransform SpawnTransform(Rotation, MuzzleFlashSocketLocation,
				FVector(1, 1, 1));
	
			AProjectileBase* ProjectileToSpawn = Cast<AProjectileBase>(
				UGameplayStatics::BeginDeferredActorSpawnFromClass(this,
					this->WeaponProjectileClass, SpawnTransform));
			ProjectileToSpawn->Init(OwnerController, ShotDirection, this, TempDamage, InBulletsCounter);
			UGameplayStatics::FinishSpawningActor(ProjectileToSpawn, SpawnTransform);
		}
	}
	// Validate trail fx particle
	if (this->TrailFx != nullptr)
	{
		// Spawning trail fx emitter start location from muzzle flash location & validate not nullptr
		if (UParticleSystemComponent* TrailEmitter = UGameplayStatics::SpawnEmitterAtLocation(
			this, this->TrailFx, MuzzleFlashSocketLocation))
		{
			// Set a vector parameter to be end location of the emitter (make emitter travel to end loc)
			// End location = end of line trace if the impact point location was not set (in case of an invalid hit)
			TrailEmitter->SetVectorParameter(FName("ShockBeamEnd"), TrailEndLocation);
		}
	}
}

// Called to stop attacking (release trigger)
void AGun::StopAttack()
{
	Super::StopAttack();
}

// Called to reload the gun
void AGun::ReloadGun()
{
	// Unsigned integer (positive bit only) to ammo available from inventory of gun type
	const uint16 AmmoToTake = GetAmmoAmountAvailableForReload();
	// Adding amounts to gun magazine and total
	this->CurrentAmmo += AmmoToTake;
	this->CurrentTotalAmmo -= AmmoToTake;
	
	if (AmmoToTake > 0)
	{
		if (this->PlayerInventory != nullptr)
		{
			// Decrease amounts from inventory
			PlayerInventory->DecreaseAmmoByAmmoType(this->WeaponType, AmmoToTake);
		}
	}

	// Update ammo in gun's weapon item object
	UWeaponItemObject* WeaponItemObject = Cast<UWeaponItemObject>(this->GetItemObject());
	if (WeaponItemObject != nullptr)
	{
		WeaponItemObject->GunObjectCurrentAmmo = this->CurrentAmmo;
		if (this->IsDualWeapons && !this->IsLeftGun)
		{
			WeaponItemObject->RightHandGunCurrentAmmo = this->CurrentAmmo;
		}
	}
	// Update ammo in widgets
	this->OnAmmoChangeDel.Broadcast(this->CurrentAmmo, this->CurrentTotalAmmo);
	
	if (const AMainGameState* MainGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(this)))
	{
		// Update widgets & availability of buying ammo from wall weapon of same gun type 
		MainGameState->UpdateWallWeaponsBuyInteractionWidgets(this->GetClass(), MaxAmmoForGun,
			this->MaxAmmoForGun - this->GetCurrentTotalAmmo());
	}
}

/*
  Method returns whether there's an extra ammo in the player's
  inventory for this type of gun or not
 */
bool AGun::HasExtraAmmo() const
{
	return this->CurrentTotalAmmo > 0;
}

// Save/ load game has been cancelled
// Called to load the gun from save slot
void AGun::LoadGun()
{
	if (!UGameplayStatics::DoesSaveGameExist("MySlot", 0)) return;
	
	const UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot("MySlot", 0));
	const TMap<FName, int32> GunsCurrentAmmoAmount = SaveGameInstance->GunsCurrentAmmo;
	const TMap<FName, int32> RightGunAmmoAmount = SaveGameInstance->RightGunsCurrentAmmo;
	if (GunsCurrentAmmoAmount.Contains(this->GetItemId()))
	{
		this->CurrentAmmo = GunsCurrentAmmoAmount[this->GetItemId()];
		if (UWeaponItemObject* WeaponItemObject = Cast<UWeaponItemObject>(this->GetItemObject()))
		{
			WeaponItemObject->GunObjectCurrentAmmo = this->CurrentAmmo;
		}
	}
	if (RightGunAmmoAmount.Contains(this->GetItemId()))
	{
		if (UWeaponItemObject* WeaponItemObject = Cast<UWeaponItemObject>(this->GetItemObject()))
		{
			WeaponItemObject->RightHandGunCurrentAmmo = RightGunAmmoAmount[this->GetItemId()];
		}
	}
}


/* Getters */

// Method returns the gun skeletal mesh component
USkeletalMeshComponent* AGun::GetSkeletalMeshComponent() const
{
	return this->SkeletalMeshComponent;
}

// Method returns whether the gun has ammo or not (in magazine)
bool AGun::GetHasAmmo() const
{
	return this->CurrentAmmo > 0;
}

// Method returns whether the gun's magazine is full or not
bool AGun::GetIsMagFull()
{
	return this->IsMagFull = this->CurrentAmmo == this->MaxMagAmmo;
}

// Method returns the reload time for the gun
float AGun::GetReloadTime() const
{
	return this->ReloadTime;
}

// Method returns the gun's reloading animation
UAnimationAsset* AGun::GetGunReloadAnimation() const
{
	return this->ReloadAnimation;
}

// Method returns the available ammo amount for reload
int32 AGun::GetAmmoAmountAvailableForReload()
{
	if (this->PlayerInventory == nullptr) return 0;
	this->SetCurrentTotalAmmo(this->PlayerInventory->GetAmmoByWeaponType(this->WeaponType));
	
	const int32 AvailableAmmoAmount =  FMath::Min(this->MaxMagAmmo, this->CurrentTotalAmmo);
	const int32 AmmoToTakeForReload = FMath::Min(this->MaxMagAmmo - this->CurrentAmmo, AvailableAmmoAmount);
	return AmmoToTakeForReload;
}

// Method returns the gun's current ammo (in magazine)
int32 AGun::GetCurrentAmmo() const
{
	return this->CurrentAmmo;
}

// Method returns the total gun's ammo (in inventory)
int32 AGun::GetCurrentTotalAmmo() const
{
	return this->CurrentTotalAmmo;
}

// Method returns gun's bullet spread amount
float AGun::GetBulletSpread() const
{
	return this->BulletSpread;
}


/* Setters */

// Method to set the bullet spread amount
void AGun::SetBulletSpread(const float InBulletSpread)
{
	this->BulletSpread = InBulletSpread;
}

// Method to set total ammo of a gun
void AGun::SetCurrentTotalAmmo(const int32 InNewTotalAmmo)
{
	this->CurrentTotalAmmo = InNewTotalAmmo;
	this->OnAmmoChangeDel.Broadcast(this->CurrentAmmo, this->CurrentTotalAmmo);
}