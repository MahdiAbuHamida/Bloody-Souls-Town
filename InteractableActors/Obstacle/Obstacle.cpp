// Mahdi Abu Hamida - Final University Project (2022)


#include "Obstacle.h"

AObstacle::AObstacle()
{
	this->InteractableType = Interactable_Obstacle;
}

// Called when the game starts or when spawned
void AObstacle::BeginPlay()
{
	Super::BeginPlay();

	if (this->StaticMeshComponent != nullptr)
	{
		this->CurrentLocation = this->StaticMeshComponent->GetRelativeLocation();
	}
}

// Called to remove the obstacle from the world
void AObstacle::RemoveObstacle()
{
	this->bIsRemoving = true;
	if (this->bRaiseObstacleUp && this->StaticMeshComponent != nullptr)
	{
		this->GetWorldTimerManager().SetTimer(this->TimerHandle_RaiseObstacleUp, this, &AObstacle::RaiseObstacleUp,
			this->RaiseRate, true);
	}
	else
	{
		this->Destroy();
	}
}

// Method returns whether the obstacle is currently getting removed or not
bool AObstacle::GetIsRemoving() const
{
	return this->bIsRemoving;
}

// Called to raise the obstacle up
void AObstacle::RaiseObstacleUp()
{
	if (this->CurrentLocation == this->TopTargetLocation)
	{
		this->GetWorldTimerManager().ClearTimer(this->TimerHandle_RaiseObstacleUp);
		this->Destroy();
	}
	else
	{
		this->CurrentLocation = FMath::VInterpConstantTo(this->CurrentLocation, this->TopTargetLocation,
			this->GetWorld()->GetDeltaSeconds(), this->InterpolationSpeed);
		this->StaticMeshComponent->SetRelativeLocation(this->CurrentLocation);
	}
}
