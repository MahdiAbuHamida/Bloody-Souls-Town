// Mahdi Abu Hamida - Final University Project (2022)


#include "MainGameMode.h"

#include "AIController.h"
#include "MainGameState.h"
#include "BehaviorTree/BlackboardComponent.h"
// #include "MyGameInstance.h"
#include "BloodySoulsTown/Characters/AI/EnemySpawner.h"
#include "BloodySoulsTown/InteractableActors/Items/QuestItem/QuestItem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Called when the game starts
void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();

	this->MainGameState = this->GetGameState<AMainGameState>();

	this->FindZombieSpawners();

	if (IsValid(this->BossEnemiesSpawnerClass))
	{
		UGameplayStatics::GetAllActorsOfClass(this, this->BossEnemiesSpawnerClass,
			this->BossSpawners);
		this->NumberOfBossEnemySpawners = this->BossSpawners.Num();
	}

	for (TSubclassOf<AQuestItem> Class : QuestItemsClasses)
	{
		if (IsValid(Class))
		{
			TArray<AActor*> Actors;
			UGameplayStatics::GetAllActorsOfClass(this, Class, Actors);
			if (Actors.Num())
			{
				const uint8_t RandomIdx = FMath::RandRange(0, Actors.Num() - 1);
				for (uint8_t i = 0; i < Actors.Num(); i++)
				{
					if (Actors[i] != nullptr && i != RandomIdx)
					{
						Actors[i]->Destroy();
					}
				}
			}
 		}
	}

	this->BreakBetweenRounds();
}

// Called to play "Round Begin" Sfx
void AMainGameMode::PlayRoundBeginSfx()
{
	this->GetWorldTimerManager().ClearTimer(this->TimerHandle_PlayRoundBeginSfx);

	if (this->RoundBeginSfx != nullptr)
	{
		UGameplayStatics::PlaySound2D(this, this->RoundBeginSfx,
			1, 1, 0, nullptr, this, false);
	}
}

// Called to play "Wolfs Round" Sfx
void AMainGameMode::PlayWolfsRoundBeginSfx()
{
	this->GetWorldTimerManager().ClearTimer(this->TimerHandle_PlayWolfsRoundBeginSfx);

	if (this->WolfsRoundBeginSfx != nullptr)
	{
		UGameplayStatics::PlaySound2D(this, this->WolfsRoundBeginSfx,
			1, 1, 0, nullptr, this, false);
	}
}

// Called to check if need to spawn a boss, if true it spawns one
void AMainGameMode::CheckSpawningBosses()
{
	const int16 CurrentRound = this->MainGameState->CurrentRound;
	/*
	  Check current round > 0 and can be divided on 8,
	  and there's at least 1 boss enemy spawner in the world has been found
	  if any of the conditions is false, return
	 */
	if (!(CurrentRound > 0 && CurrentRound % this->RoundDividerToAllowBossSpawning == 0 &&
		this->NumberOfBossEnemySpawners > 0)) return;
	const uint16 MaxRandom = FMath::Min(4, CurrentRound / this->RoundDividerToAllowBossSpawning);
	const uint16 MinRandom = MaxRandom >= 3 ? 2 : 1;
	const uint16 NumberOfBossesToSpawn = FMath::Min(FMath::RandRange(MinRandom, MaxRandom),
		this->NumberOfBossEnemySpawners);
	uint16 i = 0;
	do
	{
		++i;
		const uint16 RandomIndex = FMath::RandRange(0, NumberOfBossesToSpawn);
		if (const AEnemySpawner* EnemySpawner = Cast<AEnemySpawner>(this->BossSpawners[RandomIndex]))
		{
			EnemySpawner->SpawnAICharacter();
		}
	} while (i < NumberOfBossesToSpawn);
}

// Called before a new round begins
void AMainGameMode::BreakBetweenRounds()
{
	if (this->MainGameState == nullptr || this->IsGameOver) return;
	
	/* Save game has been cancelled */
	
	// const int32 Round = this->MainGameState->CurrentRound;
	// if (Round != 0 && Round % 10 % 9 == 0)
	// {
	// 	if (UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(this->GetGameInstance()))
	// 	{
	// 		MyGameInstance->SaveGame();
	// 		if (GEngine)
	// 		{
	// 			GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green,
	// 				FString::Printf(TEXT("Game Auto Saved")));
	// 		}
	// 	}
	// }

	this->bBreakTime = true;

	FTimerManager& TimerManager = this->GetWorldTimerManager();

	const bool bWolfsRound = this->MainGameState->CheckIfWolfsRound(this->MainGameState->CurrentRound + 1);
	
	if (bWolfsRound)
	{
		this->EnemySpawners.Empty();
		UGameplayStatics::GetAllActorsOfClass(this, this->WolfEnemiesSpawnerClass, this->EnemySpawners);
		this->NumberOfEnemySpawners = this->EnemySpawners.Num();

		TimerManager.SetTimer(this->TimerHandle_PlayWolfsRoundBeginSfx, this,
			&AMainGameMode::PlayWolfsRoundBeginSfx, this->DelayToPlayWolfsRoundBeginSfx, false);
	}
	else
	{
		TimerManager.SetTimer(this->TimerHandle_PlayRoundBeginSfx, this,
			&AMainGameMode::PlayRoundBeginSfx, this->DelayToPlayRoundBeginSfx, false);
	}
	if (this->MainGameState->CheckIfWolfsRound(this->MainGameState->CurrentRound))
	{
		this->EnemySpawners.Empty();
		this->FindZombieSpawners();
	}
	
	TimerManager.SetTimer(this->TimerHandle_BreakBetweenRounds, this, &AMainGameMode::StartNewRound,
		this->DelayBetweenRounds, false);
}

// Called to start a new round
void AMainGameMode::StartNewRound()
{
	if (this->MainGameState == nullptr || this->IsGameOver) return;
	
	this->MainGameState->IncreaseRound();
	this->bBreakTime = false;
	
	const float RandomRangeSpawningDelay = FMath::FRandRange(this->MinDelayBetweenSpawningEnemies,
		this->MaxDelayBetweenSpawningEnemies);
	
	this->GetWorldTimerManager().SetTimer(this->TimerHandle_SpawnEnemies, this, &AMainGameMode::SpawnEnemies,
		RandomRangeSpawningDelay, true);

	this->CheckSpawningBosses();
}

// Called to find "zombie" enemy spawners in the world
void AMainGameMode::FindZombieSpawners()
{
	if (this->IsGameOver) return;
	
	TArray<AActor*> EnemySpawnersFound;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemySpawner::StaticClass(), EnemySpawnersFound);
	for (AActor* Actor : EnemySpawnersFound)
	{
		if (AEnemySpawner* EnemySpawner = Cast<AEnemySpawner>(Actor))
		{
			if (!(EnemySpawner->bBossSpawner || EnemySpawner->bWolfsSpawner))
			{
				this->EnemySpawners.AddUnique(EnemySpawner);
			}
		}
	}
	this->NumberOfEnemySpawners = this->EnemySpawners.Num();
}

// Called to spawn an enemy AI or to stop timer from calling this method
void AMainGameMode::SpawnEnemies()
{
	if (this->MainGameState == nullptr || this->IsGameOver) return;
	
	if (this->MainGameState->NumberOfSpawnedEnemies < this->MainGameState->NumberOfEnemiesToSpawn)
	{
		if (this->NumberOfEnemySpawners > 0)
		{
			const int32 RandomIndex = FMath::RandRange(0, this->NumberOfEnemySpawners - 1);
			if (const AEnemySpawner* EnemySpawner = Cast<AEnemySpawner>(this->EnemySpawners[RandomIndex]))
			{
				EnemySpawner->SpawnAICharacter();
			}
		}
	}
	else
	{
		this->GetWorldTimerManager().ClearTimer(this->TimerHandle_SpawnEnemies);
	}
}

// Called to spawn an enemy spawner
bool AMainGameMode::SpawnEnemySpawner(const FVector SpawnLocation, UClass* EnemySpawnerClass)
{
	if (this->MainGameState == nullptr || this->IsGameOver) return false;
	
	if (AEnemySpawner* NewEnemySpawner = Cast<AEnemySpawner>(this->GetWorld()->SpawnActor(EnemySpawnerClass, &SpawnLocation)))
	{
		const int32 RoundToCheck = this->bBreakTime ? this->MainGameState->CurrentRound + 1 : this->MainGameState->CurrentRound;
		if (!(NewEnemySpawner->bBossSpawner || NewEnemySpawner->bWolfsSpawner || this->MainGameState->CheckIfWolfsRound(RoundToCheck)))
		{
			this->EnemySpawners.AddUnique(NewEnemySpawner);
			this->NumberOfEnemySpawners = this->EnemySpawners.Num();
		}
		if (NewEnemySpawner->bWolfsSpawner && this->MainGameState->CheckIfWolfsRound(RoundToCheck))
		{
			this->EnemySpawners.AddUnique(NewEnemySpawner);
		}
		if (NewEnemySpawner->bBossSpawner)
		{
			this->BossSpawners.AddUnique(NewEnemySpawner);
			++this->NumberOfBossEnemySpawners;
		}
		return true;
	}
	return false;
}

// Called when the game end (after the player die)
void AMainGameMode::GameOver()
{
	this->IsGameOver = true;
	
	FTimerManager& TimerManager = this->GetWorldTimerManager();
	
	if (TimerManager.IsTimerActive(this->TimerHandle_BreakBetweenRounds))
	{
		TimerManager.ClearTimer(this->TimerHandle_BreakBetweenRounds);
	}
	
	if (TimerManager.IsTimerActive(this->TimerHandle_SpawnEnemies))
	{
		TimerManager.ClearTimer(this->TimerHandle_SpawnEnemies);
	}
}
