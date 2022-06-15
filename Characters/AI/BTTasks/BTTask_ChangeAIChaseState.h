// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BloodySoulsTown/Characters/AI/Enumurations/AI_ChaseState.h"
#include "BTTask_ChangeAIChaseState.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API UBTTask_ChangeAIChaseState final : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_ChangeAIChaseState()
	{
		this->NodeName = TEXT("Task_ChangeAIChaseState");
	}

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsEnum(TEXT("AI_ChaseState"), this->ChaseState);
		return EBTNodeResult::Succeeded;
	}

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EAI_ChaseState> ChaseState;
};
