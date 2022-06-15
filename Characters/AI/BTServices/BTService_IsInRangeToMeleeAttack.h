// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BloodySoulsTown/Characters/AlexCharacter.h"
#include "BloodySoulsTown/Characters/AI/EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "BTService_IsInRangeToMeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API UBTService_IsInRangeToMeleeAttack final : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_IsInRangeToMeleeAttack()
	{
		this->NodeName = TEXT("Service_IsInRangeToMeleeAttack");
		this->bNotifyBecomeRelevant = true;
	}

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override
	{
		AAlexCharacter* PlayerCharacter = Cast<AAlexCharacter>(UGameplayStatics::GetPlayerPawn
			(this, 0));
		AEnemyCharacter* OwnerCharacter = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
		if (OwnerCharacter == nullptr || PlayerCharacter == nullptr || PlayerCharacter->IsDead())
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("CanMeleeAttack"), false);
			return;
		}

		// Assigning if AI is in a valid range to attack
		const bool bCanMeleeAttack = OwnerCharacter->GetDistanceTo(PlayerCharacter) <= OwnerCharacter->GetDamageRange();
		// Setting whether AI can melee attack or not (attack [hit] range = damage range + 1.5 player capsule radius)
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("CanMeleeAttack"), bCanMeleeAttack);
	}
};
