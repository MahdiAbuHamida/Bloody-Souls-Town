// M&M Games 2021 - Free University Project


#include "LampActor.h"

#include "Components/PointLightComponent.h"

// Sets default values
ALampActor::ALampActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Creating the light meshes (represents cylinders with an emissive material)
	this->LightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Light Mesh 1"));
	this->LightMesh->SetupAttachment(this->RootComponent);
}

// Called when the game starts or when spawned
void ALampActor::BeginPlay()
{
	Super::BeginPlay();

	// Check if flickering is set to true and flicker speeds ranges > 0 
	if (this->bFlickering && this->FlickeringSpeedsRanges.Num() > 0)
	{
		// Setting a timer to start flickering
		this->GetWorldTimerManager().SetTimer(this->TimerHandle_StartFlickering, this,
			&ALampActor::StartFlickering, FMath::FRandRange(
				this->StartFlickeringTimeRanges.X, this->StartFlickeringTimeRanges.Y), false);
	}
}

// Called to start light flickering
void ALampActor::StartFlickering()
{
	FTimerManager& TimerManager = this->GetWorldTimerManager();
	
	TimerManager.ClearTimer(this->TimerHandle_StartFlickering);

	// Check if stop flicker is set to true
	if (this->bStopFlickering)
	{
		// Setting a timer to stop flickering
		TimerManager.SetTimer(this->TimerHandle_StopFlickering, this,
		&ALampActor::StopFlickering, FMath::FRandRange(
			this->StopFlickeringTimeRanges.X, this->StopFlickeringTimeRanges.Y), false);
	}
	// Called to do flicker once
	this->Flicker();
}

// Called to do a flicker once
void ALampActor::Flicker()
{
	// Check if forced to stop flickering then return
	if (this->ForceStopFlickering) return;
	
	// Check if light mesh scale (the once with light shiny material) reached the max value (light = on)
	if (this->LightMesh->GetRelativeScale3D() == this->LightActiveMeshScale)
	{
		// Resetting the scale to inactive scale value (light = off)
		this->LightMesh->SetRelativeScale3D(this->LightInactiveMeshScale);
		// Called to decrease light intensity in blueprint child class
		this->DecreaseLightDel.Broadcast();
	}
	// Otherwise set light mesh scale to max value (light = on)
	else
	{
		this->LightMesh->SetRelativeScale3D(this->LightActiveMeshScale);
		this->IncreaseLightDel.Broadcast();
	}
		
	FTimerManager& TimerManager = this->GetWorldTimerManager();
	// Picking up a random speed range
	const int32 RandomIndex = FMath::RandRange(0, this->FlickeringSpeedsRanges.Num()-1);
	const float RandomSpeed = FMath::FRandRange(
		this->FlickeringSpeedsRanges[RandomIndex].X, this->FlickeringSpeedsRanges[RandomIndex].Y);
	// Check if timer for flicker is still active
	if (TimerManager.IsTimerActive(this->TimerHandle_Flickering))
	{
		// Clear timer for flicker
		TimerManager.ClearTimer(this->TimerHandle_Flickering);
	}
	// Setting new timer to flicker with new speed
	TimerManager.SetTimer(this->TimerHandle_Flickering, this, &ALampActor::Flicker, RandomSpeed, false);
}

// Called to stop flickering
void ALampActor::StopFlickering()
{
	// Check if flicker timer is active
	if (this->GetWorldTimerManager().IsTimerActive(this->TimerHandle_Flickering))
	{
		// Clear flicker timer
		this->GetWorldTimerManager().ClearTimer(this->TimerHandle_Flickering);
	}
	// Otherwise force to stop flicker is set to true
	else
	{
		this->ForceStopFlickering = true;
	}
}

// Method returns the static mesh component
UStaticMeshComponent* ALampActor::GetStaticMeshComponent() const
{
	return this->LightMesh;
}

