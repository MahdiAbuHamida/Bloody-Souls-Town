// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ChasePlayer.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API UBTTask_ChasePlayer final : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_ChasePlayer()
	{
		this->NodeName = TEXT("Task_ChasePlayer");
	}

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override
	{
		// Moving the AI towards the player
		OwnerComp.GetAIOwner()->MoveToLocation(OwnerComp.GetBlackboardComponent()->GetValueAsVector(
			"TargetLocation"));
		return EBTNodeResult::Succeeded;
	}
};
