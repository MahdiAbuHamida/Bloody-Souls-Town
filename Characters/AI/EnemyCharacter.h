// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class BLOODYSOULSTOWN_API AEnemyCharacter final : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	// Called to take a damage
	void CustomTakeDamage(const float DamageAmount, const FHitResult& HitResult, AActor* DamageCauser);

	// Called to perform an attack animation
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void MeleeAttack();

	// Called to try to damage the player at a keyframe of the attack animation montage
	UFUNCTION(BlueprintCallable)
	void AIApplyDamage();

	// Called to spawn a blood decals below the AI while crawling at a keyframe of the crawling animation
	UFUNCTION(BlueprintCallable)
	void SpawnBloodDecalWhileCrawling();

	
	/* Getters */

	// Method returns whether the AI is dead or not
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	// Method returns the AI overall power (speed & health average)
	UFUNCTION(BlueprintPure)
	float GetAIOverallPower() const;

	// Method returns whether the AI can crawl or not when it's power is considered very low
	UFUNCTION(BlueprintPure)
	bool GetCanCrawl() const;

	// Method returns the AI current maximum speed
	float GetEnemyMaximumSpeed() const;

	// Method returns the AI current minimum speed
	float GetEnemyMinimumSpeed() const;

	// Method returns the AI damage range
	UFUNCTION(BlueprintNativeEvent)
	float GetDamageRange();

	// Method returns the AI damage
	UFUNCTION(BlueprintPure)
	float GetAIDamage() const;

	// Method returns whether the AI is crawling or not
	bool GetIsCrawling() const;


	/* Setters */

	// Method to set the AI maximum speed
	UFUNCTION(BlueprintCallable)
	void SetAIMaxSpeedToLowest();

	// Method to set whether the AI is crawling or not
	UFUNCTION(BlueprintCallable)
	void SetIsCrawling(const bool bInIsCrawling);

	UFUNCTION(BlueprintCallable)
	void SetDamage(const float InDamage);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Blueprint event called to show a hit marker widget once
	// the AI get a hit by the player
	UFUNCTION(BlueprintImplementableEvent)
	void OnHitMarker(const bool IsDead, const float DamageAmount, const FVector EventLocation);

	UPROPERTY()
	class AMainGameState* MainGameState;

	// Max health of the AI
	UPROPERTY(EditAnywhere, Category = "AI Abilities")
	float MaxHealth = 70.0f;

	// Final max health after adding more in higher game rounds
	UPROPERTY(EditAnywhere, Category = "AI Abilities")
	float MaxHealthAfterRoundAddition = 250.0f;
	
	// AI max speed
	UPROPERTY(EditAnywhere, Category = "AI Abilities")
	float MaxEnemySpeed = 200.0f;

	float MaxEnemySpeedCopy;

	// AI min speed
	UPROPERTY(EditAnywhere, Category = "AI Abilities")
	float MinEnemySpeed = 20.0f;

	// AI damage amount
	UPROPERTY(EditAnywhere, Category = "AI Abilities")
	float AIDamage;

	// AI damage range (cm)
	UPROPERTY(EditAnywhere, Category = "AI Abilities")
	float DamageRange = 80.0f;

	// Delay before start lowering AI power (seconds)
	UPROPERTY(EditAnywhere, Category = "AI Abilities")
	float DelayToBeginLowerEnemyPower = 270.0f;

	// Max delay before start lowering AI power (seconds)
	UPROPERTY(EditAnywhere, Category = "AI Abilities")
	float MaxDelayToBeginLowerEnemyPower = 400.0f;

	// Lowering AI power rate (seconds)
	UPROPERTY(EditAnywhere, Category = "AI Abilities")
	float LowerEnemyPowerRate = 10.0f;

	// The speed to decrease of the AI for each lower_power() call
	UPROPERTY(EditAnywhere, Category = "AI Abilities")
	float SpeedToDecrease = 10.0f;

	// The max speed to decrease of the AI for each lower_power() call
	UPROPERTY(EditAnywhere, Category = "AI Abilities")
	float MaxSpeedToDecrease = 20.0f;

	// The health amount to decrease of the AI for each lower_power() call
	UPROPERTY(EditAnywhere, Category = "AI Abilities")
	float HealthToDecrease = 10.0f;

	// The max health amount to decrease of the AI for each lower_power() call
	UPROPERTY(EditAnywhere, Category = "AI Abilities")
	float MaxHealthToDecrease = 20.0f;

	UPROPERTY(EditAnywhere, Category = "AI Abilities")
	float RoundSpeedMultiplierAddition = 20;

	// Health to add to the default health per round on begin play (added health is [this value] * round)
	// e.g. Health = Health + CurrentRound * [this value] on BeginPlay()
	UPROPERTY(EditAnywhere, Category = "AI Abilities")
	int8 RoundHealthMultiplierAddition = 8;

	// Array of melee attack animation montages
	UPROPERTY(EditAnywhere, Category = "Animations")
	TArray<UAnimMontage*> MeleeAttackAnimMontages;

	// Array of blood decal materials for when AI is crawling
	UPROPERTY(EditDefaultsOnly, Category = "Materials|Crawl")
	TArray<UMaterialInterface*> CrawlingBloodDecalsCollection;

	// Crawling blood decal materials size range (Min = X, Max = Y)
	UPROPERTY(EditDefaultsOnly, Category = "Materials|Crawl")
	FVector2D DecalSizeRange = FVector2D(70.0f, 100.0f);

	// Possibility of can the AI crawl (e.g. 100 - [this value] %)
	// Range between 1 to 100, lower value is a bigger possibility to apply crawl on low AI power
	UPROPERTY(EditDefaultsOnly, Category = "Materials|Crawl")
	int32 CanCrawlPossibility = 1;
	
private:
	// Called once the AI health reach the minimum value to be considered dead
	void Die(const bool bApplyImpulse, const FVector ImpactPoint, const FName BoneName, const float ImpulseAmount);
	
	// Called to lower AI power
	UFUNCTION()
	void LowerEnemyPower();
	
	FTimerHandle TimerHandle_LowerEnemyPower;

	// Player character reference
	UPROPERTY()
	class AAlexCharacter* PlayerCharacter;
	
	// Whether this AI is a boss or not
	UPROPERTY(EditDefaultsOnly, Category = "AI Properties")
	bool bBossEnemy = false;

	// Whether this AI can crawl or not
	bool bCanCrawl = false;

	// Whether this AI is currently crawling or not
	bool bIsCrawling = false;

	// AI current health
	float HealthPercentage;
};
