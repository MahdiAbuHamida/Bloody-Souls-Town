// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API AEnemyAIController final : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	
	/* Getters */

	// Method returns the owner AI character of this AI controller
	class AEnemyCharacter* GetOwnerAICharacter() const;

private:
	// Called to setup the AI perception system
	void SetupPerceptionSystem();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the perception system receives an update
	UFUNCTION()
	void OnUpdated(TArray<AActor*> const& UpdatedActors);

	// Called to update blackboard keys values when the perception system receives an update
	void AISense(TArray<FAIStimulus> AIStimulus);

	// Behavior tree component
	UPROPERTY(EditDefaultsOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviorTreeComponent;
	
	UPROPERTY()
	AEnemyCharacter* OwnerAICharacter;
	
	UPROPERTY()
	class UAISenseConfig_Damage* DamageSense;
	
	UPROPERTY()
	class UAISenseConfig_Sight* SightSense;
	
	UPROPERTY()
	class UAISenseConfig_Hearing* HearingSense;

	UPROPERTY()
	APawn* PlayerPawn;
};
