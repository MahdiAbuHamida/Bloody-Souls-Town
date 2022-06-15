// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/InteractableActors/Items/Weapons/WeaponType.h"
#include "GameFramework/GameStateBase.h"
#include "MainGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRoundIncreased, int32, CurrentRound, int32, EnemiesNumberToSpawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyDied, int32, NumberOfAliveEnemies, int32, NumberOfDeadEnemies);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemySpawned, int32, NumberOfAliveEnemies);

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API AMainGameState final : public AGameStateBase
{
	GENERATED_BODY()

public:
	// Called to spawn an item out of an actor
	UFUNCTION(BlueprintCallable)
	void SpawnItemFromActor(class UItemObject* InItemObject, AActor* InActor, bool GroundClamp);

	// Called to deactivate a mystery box
	UFUNCTION()
	void DeactivateMysteryBox();

	// Called to increase the round
	void IncreaseRound();

	// Called when an enemy AI dies
	void EnemyDied(const bool InIsBoss);

	// Called to spawn an ammo item
	void SpawnAmmoItem(const bool IsHeadshot, const FVector Location, const FVector ForwardVector);

	// Called to increase player points after damaging an AI enemy character
	void IncreasePlayerPoints(const float DamageScale, const bool IsEnemyDead, const FVector Location,
		const FVector ForwardVector, const bool InIsBoss);

	// Called to update mystery boxes/ wall weapons buy when the player change/ release weapon
	void UpdateMysteryBoxes_WallWeaponsBuyInteractionWidgets(const UClass* WeaponClass,
		const float MaxAmmoForGun, const float AmmoLack) const;

	// Called to update mystery boxes when the player change/ release weapon
	void UpdateMysteryBoxesInteractionWidgets(const UClass* WeaponClass) const;

	// Called to update wall weapons buy when the player change/ release weapon
	void UpdateWallWeaponsBuyInteractionWidgets(const UClass* WeaponClass, const float MaxAmmoForGun,
		const float AmmoLack) const;

	// Called to check whether a round is a wolfs round or not and returns the result
	static bool CheckIfWolfsRound(const int32 Round);

	// Called to set the gamepad right stick dead zone
	UFUNCTION(BlueprintCallable, Category = "Gamepad Settings")
	void SetGamepadRightStickDeadZone(const float Value);

	// Called to set the gamepad left stick dead zone
	UFUNCTION(BlueprintCallable, Category = "Gamepad Settings")
	void SetGamepadLeftStickDeadZone(const float Value);

	
	/* Event Dispatchers */
	
	UPROPERTY(BlueprintAssignable)
	FOnRoundIncreased OnRoundIncreasedDel;
	
	UPROPERTY(BlueprintAssignable)
	FOnEnemyDied OnEnemyDiedDel;
	
	UPROPERTY(BlueprintAssignable)
	FOnEnemySpawned OnEnemySpawnedDel;
	
	UPROPERTY(BlueprintReadWrite)
	UUserWidget* FocusedWidget;

	UPROPERTY(BlueprintReadWrite)
	FText InteractKeyText;

	
	/* Enemies & Round Stats */

	// Queue of spawned AI enemy characters
	TQueue<AActor*> EnemiesSpawnedQueue;
	
	// Current round (starts at 0)
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentRound = 0;
	
	// Number of enemies to spawn in the current round
	int32 NumberOfEnemiesToSpawn = 0;
	
	// Number of the spawned enemies in the current round
	int32 NumberOfSpawnedEnemies = 0;
	
	// Number of the enemies that are running around the map (alive and, not dead)
	int32 NumberOfAliveEnemies = 0;
	
	// Number of the enemies that are dead in the current round
	int32 NumberOfDeadEnemiesCurrentRound = 0;
	
	// Number of all the enemies that are dead from the beginning until the current round (president)
	UPROPERTY(BlueprintReadOnly)
	int32 NumberOfAllDeadEnemies = 0;
	
	// Current number of the enemy dead bodies (not destroyed yet) in the map
	int32 CurrentNumberOfDeadEnemiesBodies = 0;

	// Whether current round if a wolfs round or not
	bool bWolfsRound = false;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Limit of max dead enemies bodies in queue to begin destroy (higher performance)
	UPROPERTY(EditDefaultsOnly)
	int32 LimitOfEnemiesNumberToBeginDestroy = 10;

	// Limit of max ammo items in queue to begin destroy (higher performance)
	UPROPERTY(EditDefaultsOnly)
	int32 LimitOfAmmoItemsNumberToBeginDestroy = 20;

	// Array of ammo types references used when spawning an ammo item
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Died")
	TArray<TEnumAsByte<EWeaponType>> AmmoTypes;


private:
	// Called to check whether the round ended or not
	void CheckIfRoundIsOver() const;

	// Called to activate a mystery box
	UFUNCTION()
	void ActivateRandomMysteryBox();

	FTimerHandle TimerHandle_DeactivateMysteryBox;
	
	FTimerHandle TimerHandle_ActivateRandomMysteryBox;

	UFUNCTION()
	void ClearLaughingActive()
	{
		GetWorldTimerManager().ClearTimer(LaughingActive);
	}
	
	FTimerHandle LaughingActive;

	// Queue of spawned ammo items
	TQueue<AActor*> SpawnedAmmoItemsQueue;

	int32 NumberOfSpawnedAmmoItems = 0;

	// Delay to activate a random mystery box
	UPROPERTY(EditDefaultsOnly, Category = "Mystery Box")
	float DelayToActivateMysteryBox = 15.f;

	// Delay to deactivate the current activated mystery box
	UPROPERTY(EditDefaultsOnly, Category = "Mystery Box")
	float DelayToDeactivateMysteryBox = 300.f;

	// Array of mystery boxes references in the world
	TArray<class AMysteryBoxBase*> MysteryBoxes;

	// The current activated mystery box
	int32 CurrentActivatedMysteryBoxIndex = -1;

	// Main game mode pointer reference
	UPROPERTY()
	class AMainGameMode* MainGameMode;

	// Player character pointer reference
	UPROPERTY()
	class AAlexCharacter* PlayerCharacter;

	// Player state pointer reference
	UPROPERTY()
	class AMainPlayerState* PlayerState;
};
