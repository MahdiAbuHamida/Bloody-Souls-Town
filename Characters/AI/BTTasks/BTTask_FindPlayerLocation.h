// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Kismet/GameplayStatics.h"
#include "BTTask_FindPlayerLocation.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API UBTTask_FindPlayerLocation final : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_FindPlayerLocation()
	{
		this->NodeName = TEXT("Task_FindPlayerLocation");
	}
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override
	{
		if (const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
		{
			// Updating player location info to AI
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("TargetLocation"),
				PlayerPawn->GetActorLocation());
			return EBTNodeResult::Succeeded;
		}
		return EBTNodeResult::Failed;
	}
};
