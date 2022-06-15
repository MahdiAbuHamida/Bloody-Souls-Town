// Mahdi Abu Hamida - Final University Project (2022)


#include "EnemySpawner.h"

#include "Components/ArrowComponent.h"
#include "BloodySoulsTown/Other/MainGameState.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	this->SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultScene"));
	this->SetRootComponent(this->SceneComponent);
	
	this->ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
	this->ArrowComponent->SetupAttachment(this->RootComponent);
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	this->MainGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(this));
}

// Called to spawn an AI character
void AEnemySpawner::SpawnAICharacter() const
{
	const FTransform SpawnTransform(this->GetActorRotation(), this->GetActorLocation());
	AActor* CurrentEnemyToSpawn = UGameplayStatics::BeginDeferredActorSpawnFromClass(this->GetWorld(),
		this->EnemyCharacterToSpawn, SpawnTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	if (UGameplayStatics::FinishSpawningActor(CurrentEnemyToSpawn, SpawnTransform))
	{
		if (this->MainGameState != nullptr)
		{
			++this->MainGameState->NumberOfAliveEnemies;
			if (!this->bBossSpawner)
			{
				++this->MainGameState->NumberOfSpawnedEnemies;
			}
			this->MainGameState->EnemiesSpawnedQueue.Enqueue(CurrentEnemyToSpawn);
		}
	}
}
