// Mahdi Abu Hamida - Final University Project (2022)


#include "EnemyCharacter.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BloodySoulsTown/Characters/AlexCharacter.h"
#include "BloodySoulsTown/InteractableActors/Items/Weapons/Weapon.h"
#include "BloodySoulsTown/Other/MainGameState.h"
#include "BloodySoulsTown/Other/MainPlayerState.h"
#include "BloodySoulsTown/PlayerComponents/QuestHandler/ShieldActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#define OUT

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	this->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	this->MaxEnemySpeedCopy = this->MaxEnemySpeed;
	
	this->CanCrawlPossibility = FMath::Max(this->CanCrawlPossibility, 0);
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	this->MainGameState = Cast<AMainGameState>(this->GetWorld()->GetGameState());

	this->PlayerCharacter = Cast<AAlexCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (this->MainGameState != nullptr)
	{
		const uint16 CurrentRound = this->MainGameState->CurrentRound;
		// Calculate the max health according to default + current round addition
		this->MaxHealth = FMath::Min(this->MaxHealth + CurrentRound
			* this->RoundHealthMultiplierAddition, this->MaxHealthAfterRoundAddition);

		if (!this->bBossEnemy)
		{
			if (CurrentRound >= 3)
			{
				if (FMath::RandRange(0, 10) > 7)
				{
					this->MaxEnemySpeed = this->MinEnemySpeed;
				}
			}
			else
			{
				this->MaxEnemySpeed = FMath::Min(this->MaxEnemySpeed, FMath::Max(this->MinEnemySpeed,
					CurrentRound * this->RoundSpeedMultiplierAddition));
			}
		}
		this->GetCharacterMovement()->MaxWalkSpeed = this->MaxEnemySpeed;
	}
	
	// Current health on begin play equal to max health
	this->HealthPercentage = this->MaxHealth;

	const float FirstDelay = FMath::RandRange(this->DelayToBeginLowerEnemyPower,
		this->MaxDelayToBeginLowerEnemyPower);
	// Setting a timer to lower AI power
	this->GetWorldTimerManager().SetTimer(this->TimerHandle_LowerEnemyPower, this,
		&AEnemyCharacter::LowerEnemyPower, this->LowerEnemyPowerRate, true, FirstDelay);

	// Chance that this enemy can do a zombie crawl (in case it was a normal zombie)
	this->bCanCrawl = FMath::RandRange(1, 100) > this->CanCrawlPossibility;
}

// Called once the AI health reach the minimum value to be considered dead
void AEnemyCharacter::Die(const bool bApplyImpulse, const FVector ImpactPoint, const FName BoneName,
	const float ImpulseAmount)
{
	// Check if lower AI power timer is active
	if (this->GetWorldTimerManager().IsTimerActive(this->TimerHandle_LowerEnemyPower))
	{
		// Clear the timer
		this->GetWorldTimerManager().ClearTimer(this->TimerHandle_LowerEnemyPower);
	}
	
	if (AAIController* AIController = Cast<AAIController>(this->GetController()))
	{
		// Setting that the AI is dead in the blackboard component
		AIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsDead"), true);
	}
	
	// Trying to add impulse
	if (bApplyImpulse)
	{
		if (const AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(this, 0))
		{
			const FVector DirectionVec = UKismetMathLibrary::GetDirectionUnitVector(PlayerActor->GetActorLocation(),
				this->GetActorLocation());
			// Enable physics to apply impulse
			this->GetMesh()->SetSimulatePhysics(true);
			// Adding an impulse in direction of play location to this AI location
			this->GetMesh()->AddImpulseAtLocation(DirectionVec * ImpulseAmount, ImpactPoint, BoneName);
		}
	}
	
	USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(this->GetComponentByClass(
		USkeletalMeshComponent::StaticClass()));
	if (SkeletalMeshComponent != nullptr)
	{ // Stop the running animation blueprint
		SkeletalMeshComponent->SetAnimationMode(EAnimationMode::AnimationCustomMode);
	}
	
	if (this->MainGameState != nullptr)
	{
		this->MainGameState->EnemyDied(this->bBossEnemy);
	}

	this->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->DetachFromControllerPendingDestroy();
}

// Called to lower AI power
void AEnemyCharacter::LowerEnemyPower()
{
	// Random speed to be decrease
	const float RandomSpeedDecrease = FMath::RandRange(SpeedToDecrease, MaxSpeedToDecrease);
	this->MaxEnemySpeed = FMath::Max(this->MaxEnemySpeed - (RandomSpeedDecrease - this->MainGameState->CurrentRound / 2),
		this->MinEnemySpeed);
	// Setting new max speed
	this->GetCharacterMovement()->MaxWalkSpeed = this->MaxEnemySpeed;

	// Random health to decrease
	const float RandomHealthToDecrease = FMath::RandRange(this->HealthToDecrease, this->MaxHealthToDecrease);
	// Setting new health
	this->HealthPercentage = FMath::Max(this->HealthPercentage - (RandomHealthToDecrease -
		this->MainGameState->CurrentRound / 2), 0.f);
	
	// Check if AI health reached 0 after lowered the AI health
	if (this->IsDead())
	{
		// Called as health is 0, no impulse as last damage was self health reduction
		this->Die(false, FVector(), FName(), 0.0f);
	}
}

// Called to take a damage
void AEnemyCharacter::CustomTakeDamage(const float DamageAmount, const FHitResult& HitResult, AActor* DamageCauser)
{
	// If AI is dead, no point to take a damage
	if (this->IsDead()) return;
	if (this->MainGameState == nullptr) return;

	// Validating that the damage causer is indeed a player's weapon
	AWeapon* WeaponDamageCauser = Cast<AWeapon>(DamageCauser);
	if (WeaponDamageCauser == nullptr) return;
	APawn* WeaponOwnerPawn = Cast<APawn>(DamageCauser->GetOwner());
	if (WeaponOwnerPawn == nullptr) return;
	AMainPlayerState* MainPlayerState = Cast<AMainPlayerState>(WeaponOwnerPawn->GetPlayerState());
	if (MainPlayerState == nullptr) return;

	// Increasing shots landed in player stats as it did hit the enemy
	MainPlayerState->IncreaseShotsLanded();

	// Damage scale determined by the physics part of AI character mesh (e.g. head = 100, leg = 30)
	const float DamageScale = HitResult.PhysMaterial->DestructibleDamageThresholdScale;
	// Increasing player points by a hit only & phys part addition formula
	this->MainGameState->IncreasePlayerPoints(DamageScale, false, this->GetActorLocation(), this->GetActorForwardVector(),
		this->bBossEnemy);
	// Decreasing AI current health
	this->HealthPercentage = FMath::Max(this->HealthPercentage - DamageAmount, 0.f);

	// Check if AI is dead (0 health)
	if (this->IsDead())
	{
		// Increasing player points by a kill & phys part addition formula
		this->MainGameState->IncreasePlayerPoints(DamageScale, true, this->GetActorLocation(), this->GetActorForwardVector(),
			this->bBossEnemy);
		
		if (!WeaponDamageCauser->GetIsExplosiveWeapon())
		{ // Weapon damaged the AI is not an explosive weapon, means can show a hit marker widget in the player's screen
			// with dead marker color is checked
			this->OnHitMarker(true, DamageAmount, HitResult.ImpactPoint);
		}
		// Called to let the AI act as really dead
		return this->Die(true, HitResult.ImpactPoint, HitResult.BoneName, WeaponDamageCauser->GetWeaponKillImpulse());
	}
	if (!WeaponDamageCauser->GetIsExplosiveWeapon())
	{ // with dead marker color is unchecked
		this->OnHitMarker(false, DamageAmount, HitResult.ImpactPoint);
	}
}

// Called to perform an attack animation
void AEnemyCharacter::MeleeAttack_Implementation()
{
	if (this->MeleeAttackAnimMontages.Num() == 0) return;

	// Picking a random valid index from the attack anim montages array
	const int8 RandomIndex = FMath::RandRange(0, this->MeleeAttackAnimMontages.Num() - 1);
	// Assigning and validating the anim montage at the random index
	if (UAnimMontage* AnimMontage = this->MeleeAttackAnimMontages[RandomIndex])
	{
		// Playing a random attack animation
		this->PlayAnimMontage(AnimMontage);	
	}
}

// Called to try to damage the player at a keyframe of the attack animation montage
void AEnemyCharacter::AIApplyDamage()
{
	if (PlayerCharacter == nullptr || PlayerCharacter->GetCapsuleComponent() == nullptr) return;

	// Setting up parameters for a capsule shaped line trace
	// Getting player character capsule component radius
	const float Radius = PlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	// End location of the line trace
	const FVector EndLocation = this->GetActorLocation() + GetActorForwardVector() * (this->DamageRange + Radius * 1.50f);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	// Ignoring all AI pawns in the world
	TArray<AActor*> ActorsToIgnore;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), this->StaticClass(), ActorsToIgnore);
	FHitResult Hit;
	// Firing a line trace  towards a pawn objects (AI pawns ignored)
	const bool bSuccess = UKismetSystemLibrary::SphereTraceSingleForObjects
		(this->GetWorld(), this->GetActorLocation(), EndLocation, Radius, ObjectTypes, false, ActorsToIgnore,
			EDrawDebugTrace::None, Hit, true);
	// Validating that a hit with pawn object collision has happened
	if (!bSuccess) return;
	
	// Validating that the damaged actor was the player pawn
	if (Hit.GetActor() == Cast<AActor>(PlayerCharacter))
	{
		if (GetActorForwardVector().X < PlayerCharacter->GetActorForwardVector().X
			&& PlayerCharacter->HasShield())
		{
			if (AShieldActorComponent* Shield = PlayerCharacter->GetPlayerShieldComponent())
			{
				Shield->TakeDamage(AIDamage, FDamageEvent(), GetController(), this);
				return;
			}
		}
		// Applying a damage on the player pawn
		PlayerCharacter->TakeDamage(this->AIDamage, FDamageEvent(),
			this->GetController(), this);
	}
}

// Called to spawn a blood decals below the AI while crawling at a keyframe of the crawling animation
void AEnemyCharacter::SpawnBloodDecalWhileCrawling()
{
	if (this->CrawlingBloodDecalsCollection.Num() == 0) return;

	// Picking a random blood decal material
	UMaterialInterface* RandomBloodDecal = this->CrawlingBloodDecalsCollection[FMath::RandRange
		(0, this->CrawlingBloodDecalsCollection.Num() - 1)];
	// Validating the random decal material
	if (RandomBloodDecal == nullptr) return;

	// Setting up parameters for a line trace
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	FHitResult Hit;
	const FVector StartLoc = this->GetActorLocation();
	// End location of the line trace is under the AI character by 100 cm ~
	const FVector EndLoc = FVector(StartLoc.X, StartLoc.Y, StartLoc.Z - 100.0f);
	// Firing a line trace towards static & dynamic meshes objects
	const bool bResult = UKismetSystemLibrary::LineTraceSingleForObjects(this, StartLoc, EndLoc, ObjectTypes,
		false, ActorsToIgnore, EDrawDebugTrace::None, OUT Hit, true);
	// Validating that there was a hit
	if (!bResult || !Hit.IsValidBlockingHit()) return;

	// Rotation of the hit by the normal of the mesh got the line trace hit
	const FRotator HitRotation = Hit.Normal.Rotation();
	// Rotation of the decal to be spawned
	const FRotator DecalRotation = FRotator(HitRotation.Pitch, HitRotation.Yaw, this->GetActorForwardVector().X);
	// Random size of the decal
	const float RandomSize_YZ = FMath::RandRange(this->DecalSizeRange.X, this->DecalSizeRange.Y);
	const FVector RandomSize = FVector(10.0f, RandomSize_YZ, RandomSize_YZ);
	// Spawn blood decal material at the hit impact point
	UGameplayStatics::SpawnDecalAtLocation(this, RandomBloodDecal, RandomSize,Hit.ImpactPoint,
		DecalRotation, 60.0f);
}


/* Getters */

// Method returns whether the AI is dead or not
bool AEnemyCharacter::IsDead() const
{
	return this->HealthPercentage <= 0;
}

// Method returns the AI overall power (speed & health average)
float AEnemyCharacter::GetAIOverallPower() const
{
	return (this->HealthPercentage / this->MaxHealth + this->MaxEnemySpeed / this->MaxEnemySpeedCopy) / 2.0f;
}

// Method returns whether the AI can crawl or not when it's power is considered very low
bool AEnemyCharacter::GetCanCrawl() const
{
	return this->bCanCrawl;
}

// Method returns the AI current maximum speed
float AEnemyCharacter::GetEnemyMaximumSpeed() const
{
	return this->MaxEnemySpeed;
}

// Method returns the AI current minimum speed
float AEnemyCharacter::GetEnemyMinimumSpeed() const
{
	return this->MinEnemySpeed;
}

// Method returns the AI damage range
float AEnemyCharacter::GetDamageRange_Implementation()
{
	return this->DamageRange;
}

// Method returns the AI damage
float AEnemyCharacter::GetAIDamage() const
{
	return this->AIDamage;
}

// Method returns whether the AI is crawling or not
bool AEnemyCharacter::GetIsCrawling() const
{
	return this->bIsCrawling;
}


/* Setters */

// Method to set the AI maximum speed
void AEnemyCharacter::SetAIMaxSpeedToLowest()
{
	this->MaxEnemySpeed = this->MinEnemySpeed;
	this->GetCharacterMovement()->MaxWalkSpeed = this->MaxEnemySpeed;
}

// Method to set whether the AI is crawling or not
void AEnemyCharacter::SetIsCrawling(const bool bInIsCrawling)
{
	this->bIsCrawling = bInIsCrawling;
}

void AEnemyCharacter::SetDamage(const float InDamage)
{
	this->AIDamage = InDamage;
}
