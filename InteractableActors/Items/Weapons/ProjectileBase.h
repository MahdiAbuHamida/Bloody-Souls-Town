// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

UCLASS()
class BLOODYSOULSTOWN_API AProjectileBase final : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileBase();

	// Initialize properties before running the construction script
	void Init(AController* InWeaponOwnerController, const FVector InShotDirection, class AGun* InGun, const float InDamage,
		const uint16 InBulletsCounter);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the sphere component collides with another actor
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
		const FHitResult& ImpactResult);

	// Called to spawn impact effects after collide event
	void SpawnImpactEffects(const FHitResult ImpactResult);

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* Mesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool IsCausingRadiusDamage = false;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float LifeSpan = 0.6f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Impact")
	TSubclassOf<class AWeaponImpactEffect> WeaponImpactEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Impact")
	float DamageRadius = 450.f;

	
	/*
	  Explosion camera shakes
	  higher level = more powerful shake
	 */
	
	UPROPERTY(EditAnywhere, Category = "Camera Shake On Explosives")
	TSubclassOf<UMatineeCameraShake> Lv1CameraShakeClass;
	
	UPROPERTY(EditAnywhere, Category = "Camera Shake On Explosives")
	TSubclassOf<UMatineeCameraShake> Lv2CameraShakeClass;
	
	UPROPERTY(EditAnywhere, Category = "Camera Shake On Explosives")
	TSubclassOf<UMatineeCameraShake> Lv3CameraShakeClass;
	
	UPROPERTY(EditAnywhere, Category = "Camera Shake On Explosives")
	TSubclassOf<UMatineeCameraShake> Lv4CameraShakeClass;
	
private:
	// Called to play a camera shake in projectile colliding event location
	void ExplosiveCameraShake(const FVector PlayerLocation, const float ExplosiveDistanceToPlayer);

	// Called to change projectile location based on weapon's bullet spread
	UFUNCTION()
	void ChangeLocation();
	
	FTimerHandle TimerHandle_CameraShake;

	UPROPERTY()
	AController* WeaponOwnerController;
	
	UPROPERTY()
	AGun* Gun;
	
	float DamageFromWeapon;
	
	FVector WeaponShotDirection;
	
	float BulletSpread;
	
	FTimerHandle Handle_Spread;

	uint16 BulletsCounter = 0;
};

// Called to change projectile location based on weapon's bullet spread
inline void AProjectileBase::ChangeLocation()
{
	FVector Location = this->GetActorLocation();
	Location.X += FMath::FRandRange(-this->BulletSpread/50 * this->BulletsCounter,this->BulletSpread/50 * this->BulletsCounter);
	Location.Y += FMath::FRandRange(-this->BulletSpread/50 * this->BulletsCounter,this->BulletSpread/50 * this->BulletsCounter);
	Location.Z += FMath::FRandRange(-this->BulletSpread/50 * this->BulletsCounter,this->BulletSpread/50 * this->BulletsCounter);
	this->SetActorLocation(Location);
}
