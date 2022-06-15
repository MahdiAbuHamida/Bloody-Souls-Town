// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/Characters/AI/EnemySpawner.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API AMainGameMode final : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Called before a new round begins
	UFUNCTION()
	void BreakBetweenRounds();

	// Called to spawn an enemy spawner
	bool SpawnEnemySpawner(const FVector SpawnLocation, UClass* EnemySpawnerClass);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Delay between each round
	UPROPERTY(EditDefaultsOnly, Category = "Timers")
	float DelayBetweenRounds = 15.f;

	// Max delay between spawning an enemy and the next 
	UPROPERTY(EditDefaultsOnly, Category = "Timers")
	float MaxDelayBetweenSpawningEnemies = 4.f;

	// Min delay between spawning an enemy and the next
	UPROPERTY(EditDefaultsOnly, Category = "Timers")
	float MinDelayBetweenSpawningEnemies = 1.f;

	// Delay before playing "Round Begin" Sfx
	UPROPERTY(EditDefaultsOnly, Category = "Timers")
	float DelayToPlayRoundBeginSfx = 15.f;

	// Delay before playing "Wolfs Round Begin" sfx
	UPROPERTY(EditDefaultsOnly, Category = "Timers")
	float DelayToPlayWolfsRoundBeginSfx = 15.f;

	// Wolf enemies spawner class reference
	UPROPERTY(EditDefaultsOnly, Category = "Classes|Enemies")
	TSubclassOf<AEnemySpawner> WolfEnemiesSpawnerClass;

	// Boss enemies spawner class reference
	UPROPERTY(EditDefaultsOnly, Category = "Classes|Enemies")
	TSubclassOf<AEnemySpawner> BossEnemiesSpawnerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Classes|QuestItems")
	TArray<TSubclassOf<class AQuestItem>> QuestItemsClasses; 

	// "Round Begin" Sfx
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	class USoundCue* RoundBeginSfx;

	// "Wolfs Round Begin" Sfx
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* WolfsRoundBeginSfx;
	
	UPROPERTY(EditDefaultsOnly, Category = "More")
	int32 RoundDividerToAllowBossSpawning = 8;

private:
	// Called to start a new round
	UFUNCTION()
	void StartNewRound();

	// Called when the game ends (after the player dies)
	void GameOver();
	
	// Called to find "zombie" enemy spawners in the world
	void FindZombieSpawners();

	// Called to spawn an enemy AI or to stop timer from calling this method
	void SpawnEnemies();
	
	// Called to play "Round Begin" Sfx
	UFUNCTION()
	void PlayRoundBeginSfx();

	// Called to play "Wolfs Round" Sfx
	UFUNCTION()
	void PlayWolfsRoundBeginSfx();

	// Called to check if need to spawn a boss, if true it spawns one
	void CheckSpawningBosses();

	// Main game state pointer reference
	UPROPERTY()
	AMainGameState* MainGameState;

	// Current enemy spawners array 
	UPROPERTY()
	TArray<AActor*> EnemySpawners;

	// Current boss spawners in the world
	UPROPERTY()
	TArray<AActor*> BossSpawners;

	// Current number of enemy spawners
	int32 NumberOfEnemySpawners;

	// Current number of boss enemy spawners
	int32 NumberOfBossEnemySpawners = 0;
	
	FTimerHandle TimerHandle_BreakBetweenRounds;

	FTimerHandle TimerHandle_PlayRoundBeginSfx;

	FTimerHandle TimerHandle_PlayWolfsRoundBeginSfx;
	
	FTimerHandle TimerHandle_SpawnEnemies;

	FTimerHandle TimerHandle_CheckAI_CanSeePlayer;

	// Whether the game is about to end or not (game over)
	bool IsGameOver = false;

	// Whether it is a break between round moment or not
	bool bBreakTime = false;
};
