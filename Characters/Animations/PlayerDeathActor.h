// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerDeathActor.generated.h"

UCLASS()
class BLOODYSOULSTOWN_API APlayerDeathActor final : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerDeathActor();

	// Initialize before running the constriction script
	void Init(const FPostProcessSettings& InPostProcessSettings);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* CubeMeshComponent;

	// To know the left/ right forces exact locations on the cube
	UPROPERTY(VisibleDefaultsOnly)
	class UArrowComponent* ArrowComponent;
	
	UPROPERTY(VisibleDefaultsOnly)
	class URadialForceComponent* LeftRadialForceComponent;
	UPROPERTY(VisibleDefaultsOnly)
	URadialForceComponent* RightRadialForceComponent;

	UPROPERTY(VisibleDefaultsOnly)
	class UPostProcessComponent* PostProcessComponent;

	UPROPERTY(EditDefaultsOnly)
	FPostProcessSettings PostProcessSettings;
};
