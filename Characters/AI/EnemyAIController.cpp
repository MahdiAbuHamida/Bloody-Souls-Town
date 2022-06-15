// Mahdi Abu Hamida - Final University Project (2022)


#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BloodySoulsTown/Characters/AI/EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"


AEnemyAIController::AEnemyAIController()
{
	// Called to setup AI perception system on the constructor
	this->SetupPerceptionSystem();
}

// Called to setup the AI perception system
void AEnemyAIController::SetupPerceptionSystem()
{
	// Creating AI perception component and setting it to this AI controller
	this->SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent")));

	// Creating AI sight sense
	this->SightSense = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightSense"));
	// Setting default values
	this->SightSense->PeripheralVisionAngleDegrees = 84.f;
	this->SightSense->AutoSuccessRangeFromLastSeenLocation = 1000.f;
	this->SightSense->DetectionByAffiliation.bDetectEnemies = true;
	this->SightSense->DetectionByAffiliation.bDetectNeutrals= true;
	this->SightSense->DetectionByAffiliation.bDetectFriendlies = true;

	// Creating AI hearing sense
	this->HearingSense = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingSense"));
	// Setting default values
	this->HearingSense->DetectionByAffiliation.bDetectEnemies = true;
	this->HearingSense->DetectionByAffiliation.bDetectNeutrals= true;
	this->HearingSense->DetectionByAffiliation.bDetectFriendlies = true;

	// Creating AI damage sense
	this->DamageSense = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageSense"));

	UAIPerceptionComponent* SelfPerceptionComponent = this->GetPerceptionComponent();
	if (SelfPerceptionComponent == nullptr) return;
	
	// Add sight configuration component to perception component
	SelfPerceptionComponent->ConfigureSense(*this->SightSense);

	// Add Hearing sense configuration component to perception component
	SelfPerceptionComponent->ConfigureSense(*this->HearingSense);

	// Add damage sense configuration component to perception component
	SelfPerceptionComponent->ConfigureSense(*this->DamageSense);

	// On the perception updated
	SelfPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnUpdated);
}

// Called when the game starts or when spawned
void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	// Assigning a pointer reference to the AI character attached to this AI controller
	this->OwnerAICharacter = Cast<AEnemyCharacter>(this->GetPawn());
	// Assigning a pointer reference to the player pawn
	this->PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	// Try to run the behavior tree component
	if (this->BehaviorTreeComponent != nullptr)
	{
		this->RunBehaviorTree(this->BehaviorTreeComponent);
	}
}

// Called when the perception system receives an update
void AEnemyAIController::OnUpdated(TArray<AActor*> const& UpdatedActors)
{
	for (AActor* CurrentUpdatedActor : UpdatedActors)
	{
		if (CurrentUpdatedActor != nullptr)
		{
			FActorPerceptionBlueprintInfo Info;
			this->PerceptionComponent->GetActorsPerception(CurrentUpdatedActor, OUT Info);
			this->AISense(Info.LastSensedStimuli);
		}
	}
}

// Called to update blackboard keys values when the perception system receives an update
void AEnemyAIController::AISense(TArray<FAIStimulus> AIStimulus)
{
	UBlackboardComponent* SelfBlackboardComponent = this->GetBlackboardComponent();
	if (SelfBlackboardComponent == nullptr) return;
	for (int32 i = 0; i < AIStimulus.Num(); i++)
	{
		FAIStimulus CurrentSense = AIStimulus[i];
		switch (i)
		{
			case 0:
				SelfBlackboardComponent->SetValueAsBool(TEXT("CanSeePlayer"),
					CurrentSense.WasSuccessfullySensed());
				break;
			case 1:
				if (CurrentSense.WasSuccessfullySensed() && CurrentSense.Tag == FName("AI_Noise"))
				{
					SelfBlackboardComponent->SetValueAsVector(TEXT("TargetLocation"), CurrentSense.StimulusLocation);
					SelfBlackboardComponent->SetValueAsEnum(TEXT("AI_ChaseState"), 1);
				}
				break;
			case 2:
				if (PlayerPawn != nullptr && CurrentSense.WasSuccessfullySensed())
				{
					SelfBlackboardComponent->SetValueAsVector(TEXT("TargetLocation"),
						PlayerPawn->GetActorLocation());
					SelfBlackboardComponent->SetValueAsEnum(TEXT("AI_ChaseState"), 1);
				}
				break;
			default: ;
		}
	}
}


/* Getters */

// Method returns the owner AI character of this AI controller
AEnemyCharacter* AEnemyAIController::GetOwnerAICharacter() const
{
	return this->OwnerAICharacter;
}
