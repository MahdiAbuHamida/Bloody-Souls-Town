// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "WeaponImpactEffect.generated.h"

class USoundCue;

UCLASS()
class BLOODYSOULSTOWN_API AWeaponImpactEffect final : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponImpactEffect();

	// Initialize properties before running the construction script
	void Init(const FHitResult Hit, const bool InIsUsingHitResult);
	
	FHitResult HitResult;
	
	bool IsUsingHitResult;
	
	FRotator DecalRotation;

	UPROPERTY(EditDefaultsOnly, Category = "Decal Values")
	float DecalSize = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Decal Values")
	float DecalLife = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Decal Values")
	float BloodDecalsLife = 60.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Decal Values")
	FVector2D BloodDecalsSizeRange = FVector2D(70.0f, 100.0f);

	
	// Defaults
	
	UPROPERTY(EditDefaultsOnly, Category = "Particles|Default")
	UParticleSystem* DefaultFX;
	
	UPROPERTY(EditDefaultsOnly, Category = "SFX|Default")
	USoundCue* DefaultSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Materials|Default")
	UMaterialInterface* DefaultDecalMaterial;


	// Customs
	
	// Concrete effects
	
	UPROPERTY(EditDefaultsOnly, Category = "Particles|Concrete")
	UParticleSystem* ConcreteFX;

	UPROPERTY(EditDefaultsOnly, Category = "SFX|Concrete")
	USoundCue* ConcreteSound;

	UPROPERTY(EditDefaultsOnly, Category = "Materials|Concrete")
	UMaterialInterface* ConcreteDecalMaterial;

	
	// Flesh effects
	
	UPROPERTY(EditDefaultsOnly, Category = "Particles|Flesh")
	UParticleSystem* FleshFX;

	UPROPERTY(EditDefaultsOnly, Category = "SFX|Flesh")
	USoundCue* FleshSound;

	UPROPERTY(EditDefaultsOnly, Category = "Materials|Flesh")
	TArray<UMaterialInterface*> FleshDecalMaterialCollection;

	
	// Water effects
	
	UPROPERTY(EditDefaultsOnly, Category = "Particles|Water")
	UParticleSystem* WaterFX;

	UPROPERTY(EditDefaultsOnly, Category = "SFX|Water")
	USoundCue* WaterSound;

	UPROPERTY(EditDefaultsOnly, Category = "Materials|Water")
	UMaterialInterface* WaterDecalMaterial;

	
	// Wood effects
	
	UPROPERTY(EditDefaultsOnly, Category = "Particles|Wood")
	UParticleSystem* WoodFx;

	UPROPERTY(EditDefaultsOnly, Category = "SFX|Wood")
	USoundCue* WoodSound;

	UPROPERTY(EditDefaultsOnly, Category = "Materials|Wood")
	UMaterialInterface* WoodDecalMaterial;
	
	
	// Metal effects
	
	UPROPERTY(EditDefaultsOnly, Category = "Particles|Metal")
	UParticleSystem* MetalFx;

	UPROPERTY(EditDefaultsOnly, Category = "SFX|Metal")
	USoundCue* MetalSound;

	UPROPERTY(EditDefaultsOnly, Category = "Materials|Metal")
	UMaterialInterface* MetalDecalMaterial;
	
	
	// Extras

	UPROPERTY(EditDefaultsOnly, Category = "Particles|Explosion")
	UParticleSystem* ExplosionFX;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Called to destroy the impact effect actor after 15 seconds
	void DestroyTimer();

	/*
	  Called when a blood particle collides with another object (like a ground or walls),
	  to spawn a blood decal on the surface that collided with the blood particle
	 */
	UFUNCTION()
	void OnBloodParticleCollide(FName EventName, float EmitterTime, int32 ParticleTime, FVector Location,
		FVector Velocity, FVector Direction, FVector Normal, FName BoneName);
};