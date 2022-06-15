// Mahdi Abu Hamida - Final University Project (2022)


#include "WeaponImpactEffect.h"

#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

// Sets default values
AWeaponImpactEffect::AWeaponImpactEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Initialize properties before running the construction script
void AWeaponImpactEffect::Init(const FHitResult Hit, const bool InIsUsingHitResult)
{
	this->HitResult = Hit;
	this->IsUsingHitResult = InIsUsingHitResult;
}

// Called when the game starts or when spawned
void AWeaponImpactEffect::BeginPlay()
{
	Super::BeginPlay();

	// Validating the hit result
	if (!this->HitResult.IsValidBlockingHit() || this->HitResult.GetComponent() == nullptr)
	{
		this->Destroy();
		return;
	}
	
	const FRotator Rotation = this->HitResult.Normal.Rotation();
	this->DecalRotation = FRotator(Rotation.Pitch, Rotation.Yaw, FMath::RandRange(-180, 180));
	// The physical surface which determines what effects we need to spawn/ play
	const EPhysicalSurface PhysicalSurface = this->HitResult.PhysMaterial.Get()->SurfaceType;
	const FVector_NetQuantize Loc = this->HitResult.ImpactPoint;

	// Initialize effects with default values
	UParticleSystem* EmitterToSpawn = this->DefaultFX;
	USoundCue* SoundToPlay = this->DefaultSound;
	UMaterialInterface* MaterialToApply = this->DefaultDecalMaterial;

	// Assign effects based on the surface type
	switch (PhysicalSurface)
	{
		case SurfaceType_Default:
			{
				EmitterToSpawn = this->DefaultFX;
				SoundToPlay = this->DefaultSound;
				MaterialToApply = this->DefaultDecalMaterial;
				break;
			}
		case SurfaceType1:
			{
				EmitterToSpawn = this->ConcreteFX;
				SoundToPlay = this->ConcreteSound;
				MaterialToApply = this->ConcreteDecalMaterial;
				break;
			}
		case SurfaceType4:
			{
				EmitterToSpawn = this->WoodFx;
				SoundToPlay = this->WoodSound;
				MaterialToApply = this->ConcreteDecalMaterial;
				break;
			}
		case SurfaceType5:
			{
				EmitterToSpawn = this->MetalFx;
				SoundToPlay = this->MetalSound;
				MaterialToApply = this->ConcreteDecalMaterial;
				break;
			}
		case SurfaceType6:
			{
				EmitterToSpawn = this->FleshFX;
				SoundToPlay = this->FleshSound;
				break;
			}
		case SurfaceType11:
			{
				EmitterToSpawn = this->WaterFX;
				SoundToPlay = this->WaterSound;
				MaterialToApply = this->WaterDecalMaterial;
				break;
			}
	}

	if (SoundToPlay != nullptr)
	{
		// Playing sound effect
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundToPlay, Loc);
	}
	
	if (MaterialToApply != nullptr)
	{
		// Spawn a decal material
		UDecalComponent* DecalToSpawn = UGameplayStatics::SpawnDecalAttached(MaterialToApply,
			FVector(DecalSize, DecalSize, DecalSize), this->HitResult.GetComponent(),
			this->HitResult.BoneName, Loc, this->DecalRotation, EAttachLocation::KeepWorldPosition,
			this->DecalLife);
		DecalToSpawn->SetFadeScreenSize(0.0000001);
	}
	
	if (EmitterToSpawn != nullptr)
	{
		// Explosion particle effect spawns besides the other particle effect
		if (this->ExplosionFX != nullptr)
		{
			UGameplayStatics::SpawnEmitterAtLocation(this, this->ExplosionFX, Loc);
		}
		UParticleSystemComponent* SpawnedEmitter = UGameplayStatics::SpawnEmitterAtLocation(this,
			EmitterToSpawn, Loc);
		if (SpawnedEmitter != nullptr && EmitterToSpawn == this->FleshFX)
		{
			// Bind event to on particle collide
			TScriptDelegate<FWeakObjectPtr> ParticleCollideDelegate;
			ParticleCollideDelegate.BindUFunction(this, FName("OnBloodParticleCollide"));
			SpawnedEmitter->OnParticleCollide.AddUnique(ParticleCollideDelegate);
		}
		else
		{
			this->Destroy();
		}
	}
	else
	{
		this->Destroy();
	}

	// if (&this->TimerHandle_DestroyWeaponImpactEffect != nullptr)
	// {
	// 	// Setting a timer to destroy the weapon impact effect actor after 15 seconds (better performance)
	// 	this->GetWorldTimerManager().SetTimer(this->TimerHandle_DestroyWeaponImpactEffect, this,
	// 		&AWeaponImpactEffect::DestroyTimer, 15.0f, false);
	// }
}

// Called to destroy the impact effect actor after 15 seconds
void AWeaponImpactEffect::DestroyTimer()
{
	// this->GetWorldTimerManager().ClearTimer(this->TimerHandle_DestroyWeaponImpactEffect);
	// this->Destroy();
}

/*
  Called when a blood particle collides with another object (like a ground or walls),
  to spawn a blood decal on the surface that collided with the blood particle
 */
void AWeaponImpactEffect::OnBloodParticleCollide(FName EventName, float EmitterTime, int32 ParticleTime, FVector Location,
	FVector Velocity, FVector Direction, FVector Normal, FName BoneName)
{
	if (this->FleshDecalMaterialCollection.Num() == 0) return;

	const int8 RandomIndex = FMath::RandRange(0, this->FleshDecalMaterialCollection.Num() - 1);
	UMaterialInterface* BloodDecalMaterial = this->FleshDecalMaterialCollection[RandomIndex];
	if (BloodDecalMaterial == nullptr) return;
	const float RandomSizeY_Z = FMath::RandRange(this->BloodDecalsSizeRange.X, this->BloodDecalsSizeRange.Y);
	UGameplayStatics::SpawnDecalAtLocation(this, BloodDecalMaterial,
		FVector(10.0f, RandomSizeY_Z, RandomSizeY_Z), Location, Normal.Rotation(), this->BloodDecalsLife);

	this->Destroy();
}
