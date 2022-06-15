// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BloodySoulsTown/Characters/AI/EnemyCharacter.h"
#include "BTTask_AIMeleeAttack.generated.h"


UCLASS()
class BLOODYSOULSTOWN_API UBTTask_AIMeleeAttack final : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_AIMeleeAttack()
	{
		this->NodeName = TEXT("Task_AIMeleeAttack");
	}

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override
	{
		if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn()))
		{
			// Called to perform attack anim montage towards the player
			EnemyCharacter->MeleeAttack();
			return EBTNodeResult::Succeeded;
		}
		return EBTNodeResult::Failed;
	}
};
