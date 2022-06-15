// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerBoxEnemySpawner.generated.h"

UCLASS()
class BLOODYSOULSTOWN_API ATriggerBoxEnemySpawner final : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATriggerBoxEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Box collision component
	UPROPERTY(EditAnywhere, Category = "Actor Components")
	class UBoxComponent* TriggerBoxComp;

	// Arrow component for where to spawn the enemy spawner in the world
	UPROPERTY(EditAnywhere, Category = "Actor Components")
	class UArrowComponent* ArrowComp;
	
	// Enemy spawner class
	UPROPERTY(EditAnywhere, Category = "Enemy Spawner Properties")
	TSubclassOf<class AEnemySpawner> EnemySpawnerClass;

private:
	// Called when other actor collides with the box component
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
