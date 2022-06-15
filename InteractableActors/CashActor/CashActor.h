// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/InteractableActors/Interactable.h"
#include "CashActor.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API ACashActor final : public AInteractable
{
	GENERATED_BODY()
	
public:
	ACashActor()
	{
		this->InteractableType = EInteractableActorType::Interactable_Cash;
	}

	// Method returns points amount to be given to the player
	int32 GetPointsToGive() const
	{
		return this->PointsToGiveThePlayer;
	}

protected:
	UPROPERTY(EditAnywhere, Category = "Properties")
	int32 PointsToGiveThePlayer;
};
