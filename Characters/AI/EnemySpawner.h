// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class BLOODYSOULSTOWN_API AEnemySpawner final : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

	// Called to spawn an AI character
	UFUNCTION()
	void SpawnAICharacter() const;

	// The class (type) of the AI character to be spawned
	UPROPERTY(EditAnywhere, Category = "Enemy Spawner Properties", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APawn> EnemyCharacterToSpawn;

	// Whether this is a boss AI spawner ot not 
	UPROPERTY(EditAnywhere, Category = "Enemy Spawner Properties", meta = (AllowPrivateAccess = "true"))
	bool bBossSpawner = false;

	// Whether this is a wolf type AI spawner or not
	UPROPERTY(EditAnywhere, Category = "Enemy Spawner Properties", meta = (AllowPrivateAccess = "true"))
	bool bWolfsSpawner = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComponent;
	
	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* ArrowComponent;

	UPROPERTY()
	class AMainGameState* MainGameState;
};
