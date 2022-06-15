// Mahdi Abu Hamida - Final University Project (2022)


#include "AimAssistComponent.h"

#include "BloodySoulsTown/Characters/AI/EnemyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UAimAssistComponent::UAimAssistComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UAimAssistComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const AActor* Owner = this->GetOwner())
	{
		this->PlayerCameraComp = Cast<UCameraComponent>(Owner->GetComponentByClass(UCameraComponent::StaticClass()));
	}
	this->OwnerController = UGameplayStatics::GetPlayerController(this->GetWorld(), 0);
	
}

// Called to trigger aim assist
void UAimAssistComponent::StartAimAssist(const float InterpolationSpeed, const float Range)
{
	/*
	 Check if player controller is not null pointer
	 and fire 2 Aim assist line traces, 1st with 8 radius,
	 2nd with 70 radius, check both for a hit result returns true
	 and check that the hit result actor is not null pointer
	 */
	if (this->OwnerController == nullptr || !this->AimAssistTrace(8.f, Range)
		|| !this->AimAssistTrace(70.0f, Range) || this->Hit.GetActor() == nullptr) return;

	// Check if hit actor is AI character
	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(this->Hit.GetActor());
	if (EnemyCharacter == nullptr) return;

	// If AI is crawling then target aim z value is -50, otherwise +50
	const float Z = EnemyCharacter->GetIsCrawling() ? -50.0f : 50.0f;
	
	const FVector HitActorLocation = this->Hit.GetActor()->GetActorLocation();
	const FVector NewLocation(HitActorLocation.X, HitActorLocation.Y, HitActorLocation.Z + Z);
	const FVector FinalLoc = (NewLocation-this->Hit.ImpactPoint).GetSafeNormal(0.0001f);

	// Moving the player controller control rotation to the rotation of the final target location
	this->OwnerController->SetControlRotation(UKismetMathLibrary::RInterpTo(this->OwnerController->GetControlRotation(),
		FinalLoc.Rotation(), GetWorld()->GetDeltaSeconds(), InterpolationSpeed));
}

// Called to check for enemy AI pawns to start aim assist
bool UAimAssistComponent::AimAssistTrace(const float Radius, const float Range)
{
	if (this->PlayerCameraComp == nullptr) return false;

	// Line trace starts from player camera location in world space (not relative)
	const FVector StartLocation = this->PlayerCameraComp->GetComponentLocation();
	// Line trace ends with weapon range addition
	const FVector EndLocation = StartLocation + this->PlayerCameraComp->GetForwardVector() * Range;
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this->GetOwner());

	// Firing the line trace towards pawn objects (AI pawn) with player is ignored
	return UKismetSystemLibrary::CapsuleTraceSingleForObjects(this->GetWorld(), StartLocation, EndLocation,
		Radius, Radius, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None,
		this->Hit, true);
}