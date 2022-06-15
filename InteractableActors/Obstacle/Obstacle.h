// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/InteractableActors/Interactable.h"
#include "Obstacle.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API AObstacle final : public AInteractable
{
	GENERATED_BODY()

public:
	AObstacle();

	// Called to remove the obstacle from the world
	void RemoveObstacle();

	// Method returns whether the obstacle is currently getting removed or not
	bool GetIsRemoving() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Whether to raise the obstacle up or not (when is being removed)
	UPROPERTY(EditAnywhere, Category = "booleans")
	bool bRaiseObstacleUp = false;

	// Top target location for the obstacle before gets destroyed
	UPROPERTY(EditAnywhere, Category = "Raise Values")
	FVector TopTargetLocation = FVector(0.0f, 0.0f, 0.0f);

	// Raise obstacle up rate (seconds)
	UPROPERTY(EditAnywhere, Category = "Raise Values")
	float RaiseRate = 0.05f;

	// Obstacle raise up interpolation speed
	UPROPERTY(EditAnywhere, Category = "Raise Values")
	float InterpolationSpeed = 2.0f;
	
private:
	// Called to raise the obstacle up
	UFUNCTION()
	void RaiseObstacleUp();

	FTimerHandle TimerHandle_RaiseObstacleUp;

	// Current location of the obstacle
	FVector CurrentLocation;

	// Whether the obstacle is currently getting removed or not
	bool bIsRemoving;
};
