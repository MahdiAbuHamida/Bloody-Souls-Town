// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AimAssistComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLOODYSOULSTOWN_API UAimAssistComponent final : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAimAssistComponent();

	// Called to trigger aim assist
	void StartAimAssist(const float InterpolationSpeed, const float Range);
	
	// Called to check if any pawn exists in front of player location
	bool AimAssistTrace(const float Radius, const float Range);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class UCameraComponent* PlayerCameraComp;

	UPROPERTY()
	APlayerController* OwnerController;
	
	// Holds the hit result of the aim assist line trace
	FHitResult Hit;
};
