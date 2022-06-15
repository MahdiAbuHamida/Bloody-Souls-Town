// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTTask_FindRandomLocation.generated.h"


UCLASS()
class BLOODYSOULSTOWN_API UBTTask_FindRandomLocation final : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_FindRandomLocation()
	{
		this->NodeName = TEXT("Task_FindRandomLocation");
	}

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override
	{
		if (const APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn())
		{
			FVector RandomGeneratedLocation;
			// Try to find random valid surface location
			const bool bSuccess = UNavigationSystemV1::K2_GetRandomReachablePointInRadius(
				this->GetWorld(), OwnerPawn->GetActorLocation(), OUT RandomGeneratedLocation,
				this->RandomLocationRadius);
			// Check if succeeded to find random location
			if (bSuccess)
			{
				// Updating AI target move to location to random location result
				OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("TargetLocation"), RandomGeneratedLocation);
				return EBTNodeResult::Succeeded;
			}
			return EBTNodeResult::Failed;
		}
		return EBTNodeResult::Failed;
	}

	UPROPERTY(EditAnywhere)
	float RandomLocationRadius = 1000.f;
};
