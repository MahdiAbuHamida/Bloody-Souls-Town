// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BloodySoulsTown/Characters/AI/EnemyAIController.h"
#include "BloodySoulsTown/Characters/AI/EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BTService_AdjustSpeed.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API UBTService_AdjustSpeed final : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_AdjustSpeed()
	{
		this->NodeName = TEXT("Service_AdjustSpeed");
		this->bNotifyBecomeRelevant = true;
	}

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override
	{
		Super::OnBecomeRelevant(OwnerComp, NodeMemory);
		
		if (const AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner()))
		{
			if (const AEnemyCharacter* EnemyCharacter = EnemyAIController->GetOwnerAICharacter())
			{
				const float EnemyMaximumSpeed = EnemyCharacter->GetEnemyMaximumSpeed();
				// Speed = random between: A=(max value of => max speed - 50 , minimum speed) , B=(max speed)
				const float NewSpeed = FMath::FRandRange(FMath::Max(EnemyMaximumSpeed - 50.0f,
					EnemyCharacter->GetEnemyMinimumSpeed()),EnemyMaximumSpeed);
				// Updating ai enemy character max walk speed
				EnemyAIController->GetOwnerAICharacter()->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
			}
		}
	}
};
