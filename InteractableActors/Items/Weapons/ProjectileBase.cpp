// Mahdi Abu Hamida - Final University Project (2022)


#include "ProjectileBase.h"

#include "Gun.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WeaponImpactEffect.h"
#include "BloodySoulsTown/Characters/AI/EnemyCharacter.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	this->CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	this->CollisionSphere->BodyInstance.SetCollisionProfileName("Projectile");
	this->SetRootComponent(this->CollisionSphere);
	this->CollisionSphere->bReturnMaterialOnMove = true;
	this->CollisionSphere->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);

	this->Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	this->Mesh->SetupAttachment(this->CollisionSphere);

	this->ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));

	this->DamageFromWeapon = 0.0f;
	this->BulletSpread = 0.0f;
}

// Initialize properties before running the construction script
void AProjectileBase::Init(AController* InWeaponOwnerController, const FVector InShotDirection, AGun* InGun,
	const float InDamage, const uint16 InBulletsCounter)
{
	this->WeaponOwnerController = InWeaponOwnerController;
	this->WeaponShotDirection = InShotDirection;
	this->DamageFromWeapon = InDamage;
	this->Gun = InGun;
	if (this->Gun != nullptr)
	{
		this->BulletSpread = this->Gun->GetBulletSpread();
	}
	this->BulletsCounter = InBulletsCounter + 1;
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	this->GetWorldTimerManager().SetTimer(this->Handle_Spread, this, &AProjectileBase::ChangeLocation, 0.04, true);
	this->SetLifeSpan(this->LifeSpan);
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called when the sphere component collides with another actor
void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& ImpactResult)
{
	// Check if projectile does not cause an explosive damage (radius based damage) 
	if (!this->IsCausingRadiusDamage)
	{
		AActor* HitActor = ImpactResult.GetActor();
		if (HitActor != nullptr)
		{
			// Check if actor got hit is an AI enemy character
			if (AEnemyCharacter* HitAICharacter = Cast<AEnemyCharacter>(HitActor))
			{
				// Damaging the enemy
				HitAICharacter->CustomTakeDamage(this->DamageFromWeapon, ImpactResult, this->Gun);
			}
		}
	}
	/*
	  Otherwise, get all actors overlapping the sphere (e.g. v = 4/3πr^3 = sphere collision volume),
	  on the projectile colliding event location
	 */
	else
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(Cast<AActor>(this->Gun));
		TArray<AActor*> HitActors;
		UKismetSystemLibrary::SphereOverlapActors(this->GetWorld(), ImpactResult.ImpactPoint, this->DamageRadius,
		ObjectTypes, nullptr, ActorsToIgnore, HitActors);
		const FPointDamageEvent ExplosiveDamageEvent(this->DamageFromWeapon, ImpactResult, this->WeaponShotDirection,
			nullptr);
		// Iterating on all actors collides in sphere volume
		for (AActor* CurrentActor : HitActors)
		{
			// Check if actor in sphere is an enemy AI character 
			if (AEnemyCharacter* HitAICharacter = Cast<AEnemyCharacter>(CurrentActor))
			{
				// Check if enemy AI is *not* dead before damaging  
				if (!HitAICharacter->IsDead())
				{
					/*
					  Damage based to the distance between the actor was hit and this projectile location,
					  Divided by sqrt of the distance, then Multiplied by 1
				      because this actor is an enemy, the damage would be bigger
					  than the damage if let's say it was the player damaged him self
					  (x5 for the player, or anything else)
					 */
					const float Distance = CurrentActor->GetDistanceTo(this)/100.f;
					if (Distance >= 1)
					{
						this->DamageFromWeapon /= FMath::Sqrt(Distance);
					}
					else
					{
						this->DamageFromWeapon /= Distance;
					}
					// Damaging the enemy
					HitAICharacter->CustomTakeDamage(this->DamageFromWeapon, ImpactResult, this->Gun);
				}
			}
			// Check if actor in sphere is the player (the owner of the gun - damage causer) 
			else if (this->Gun != nullptr && this->Gun->GetOwner() == CurrentActor)
			{
				const float Distance = CurrentActor->GetDistanceTo(this)/100.f;
				if (Distance >= 1)
				{
					this->DamageFromWeapon /= FMath::Sqrt(Distance) * 5.f;
				}
				else
				{
					this->DamageFromWeapon /= Distance * 5.f;
				}
				// Damaging the player character
				CurrentActor->TakeDamage(this->DamageFromWeapon, ExplosiveDamageEvent, this->WeaponOwnerController,
					this->Gun);
			}
		}
	}

	// Spawn impact effects
	this->SpawnImpactEffects(ImpactResult);
}

// Called to spawn impact effects after collide event
void AProjectileBase::SpawnImpactEffects(const FHitResult ImpactResult)
{
	// Validating the projectile - weapon impact effect class first
	if (!IsValid(this->WeaponImpactEffectClass)) this->Destroy();

	// Impact effect spawn location is on the projectile collide event location point
	const FTransform SpawnTransform(ImpactResult.ImpactPoint);
	AWeaponImpactEffect* WeaponImpactEffectActorToSpawn =
		Cast<AWeaponImpactEffect>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this,
			this->WeaponImpactEffectClass, SpawnTransform));
	if (WeaponImpactEffectActorToSpawn != nullptr)
	{
		// Initialize impact effect before running the construction script
		WeaponImpactEffectActorToSpawn->Init(ImpactResult, ImpactResult.IsValidBlockingHit());
		// Finish spawning
		UGameplayStatics::FinishSpawningActor(WeaponImpactEffectActorToSpawn, SpawnTransform);
	}

	/*
	  Check if player pawn is nullptr (after - might have - got a damage of explosion)
	  or validate if damage type is *not* explosion, in that case cannot play
	  explosion camera shake on the player location
	 */
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn == nullptr || WeaponImpactEffectActorToSpawn == nullptr ||
		!this->Gun->GetIsExplosiveWeapon()) this->Destroy();
	
	const float ExplosiveDistanceToPlayer = PlayerPawn->GetDistanceTo(WeaponImpactEffectActorToSpawn)/100;
	this->ExplosiveCameraShake(PlayerPawn->GetActorLocation(), ExplosiveDistanceToPlayer);
}

// Called to play a camera shake in projectile colliding event location
void AProjectileBase::ExplosiveCameraShake(const FVector PlayerLocation, const float ExplosiveDistanceToPlayer)
{
	// Validate *all* camera shakes classes
	if (!IsValid(this->Lv1CameraShakeClass) || !IsValid(this->Lv2CameraShakeClass) || !IsValid(this->Lv3CameraShakeClass)
		|| !IsValid(this->Lv4CameraShakeClass)) this->Destroy();
	
	/*
	 * Distance in meters
	 * Playing a camera shake based on the distance between the player and the explosion event location
	 */
	
	if (ExplosiveDistanceToPlayer <= 7)
	{
		UGameplayStatics::PlayWorldCameraShake(this->GetWorld(), this->Lv1CameraShakeClass, PlayerLocation,
			0.f, 500.f, 1.f, false);
	}
	if (ExplosiveDistanceToPlayer > 7 && ExplosiveDistanceToPlayer <= 8)
	{
		UGameplayStatics::PlayWorldCameraShake(this->GetWorld(), this->Lv2CameraShakeClass, PlayerLocation,
			0.f, 500.f, 1.f, false);
	}
	if (ExplosiveDistanceToPlayer > 8 && ExplosiveDistanceToPlayer <= 9)
	{
		UGameplayStatics::PlayWorldCameraShake(this->GetWorld(), this->Lv3CameraShakeClass, PlayerLocation,
			0.f, 500.f, 1.f, false);
	}
	if (ExplosiveDistanceToPlayer > 9 && ExplosiveDistanceToPlayer <= 13)
	{
		UGameplayStatics::PlayWorldCameraShake(this->GetWorld(), this->Lv4CameraShakeClass, PlayerLocation,
			0.f, 500.f, 1.f, false);
	}
	// * Projectile functionality ends here *
	this->Destroy();
}
