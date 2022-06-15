// M&M Games 2021 - Free University Project

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LampActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FIncreaseLightIntensity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDecreaseLightIntensity);

UCLASS()
class BLOODYSOULSTOWN_API ALampActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALampActor();

	
	/* Getters */

	// Method returns the static mesh component
	UFUNCTION(BlueprintPure)
	UStaticMeshComponent* GetStaticMeshComponent() const;
	

	/* Blueprint event dispatchers */
	
	UPROPERTY(BlueprintAssignable)
	FIncreaseLightIntensity IncreaseLightDel;
	
	UPROPERTY(BlueprintAssignable)
	FDecreaseLightIntensity DecreaseLightDel;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	/* Components */
	
	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* LightMesh;

	
	/* Runtime functional properties */

	// The scale of the light mesh when light is on
	UPROPERTY(EditAnywhere, Category = "Runtime Properties")
	FVector LightActiveMeshScale;

	// The scale of the light mesh when light is off
	UPROPERTY(EditAnywhere, Category = "Runtime Properties")
	FVector LightInactiveMeshScale;

	// Whether to flicker the light on/ off or not
	UPROPERTY(EditAnywhere, Category = "Runtime Properties")
	bool bFlickering = false;

	// Whether to stop flickering the light or not
	UPROPERTY(EditAnywhere, Category = "Runtime Properties")
	bool bStopFlickering = false;

	// X = min flickering starting time, Y = max flickering starting time
	UPROPERTY(EditAnywhere, Category = "Runtime Properties")
	FVector2D StartFlickeringTimeRanges;

	// X = min flickering stopping time, Y = max flickering stopping time
	UPROPERTY(EditAnywhere, Category = "Runtime Properties")
	FVector2D StopFlickeringTimeRanges;

	// Flicker light on/ off speeds (in seconds)
	UPROPERTY(EditAnywhere, Category = "Runtime Properties")
	TArray<FVector2D> FlickeringSpeedsRanges;

private:
	// Called to start light flickering
	UFUNCTION()
	void StartFlickering();
	
	FTimerHandle TimerHandle_StartFlickering;

	// Called to do a flicker once
	UFUNCTION()
	void Flicker();
	
	FTimerHandle TimerHandle_Flickering;

	// Called to stop flickering
	UFUNCTION()
	void StopFlickering();
	
	FTimerHandle TimerHandle_StopFlickering;

	bool ForceStopFlickering = false;
};
