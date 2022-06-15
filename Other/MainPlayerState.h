// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Structs/PlayerStats.h"
#include "MainPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int32, Score);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillsIncreased, int32, Kills);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHeadshotKillsIncreased, int32, HeadshotKills);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAccuracyChanged, int32, Accuracy);

UCLASS()
class BLOODYSOULSTOWN_API AMainPlayerState final : public APlayerState
{
	GENERATED_BODY()

public:
	AMainPlayerState()
	{
		PlayerStats.Rounds = 0;
		PlayerStats.Score = 0;
		PlayerStats.Kills = 0;
		PlayerStats.HeadshotKills = 0;
		PlayerStats.Accuracy = 0;
		PlayerStats.ShotsFired = 0;
		PlayerStats.ShotsLanded = 0;
	}

	void SetScore(const int32 InScore)
	{
		PlayerStats.Score = InScore;
		OnScoreChanged.Broadcast(InScore);
	}

	void IncreaseKills()
	{
		OnKillsIncreased.Broadcast(++PlayerStats.Kills);
	}

	void IncreaseHeadshotKills()
	{
		OnHeadshotKillsIncreased.Broadcast(++PlayerStats.HeadshotKills);
	}

	void IncreaseShotsFired()
	{
		++PlayerStats.ShotsFired;
		UpdatePlayerAccuracy();
	}

	void IncreaseShotsLanded()
	{
		++PlayerStats.ShotsLanded;
		UpdatePlayerAccuracy();
	}

	void UpdatePlayerAccuracy()
	{
		if (PlayerStats.ShotsFired == 0) return;
		
		const float ShotsFired = PlayerStats.ShotsFired;
		const float ShotsLanded = PlayerStats.ShotsLanded;

		PlayerStats.Accuracy = ShotsLanded / ShotsFired * 100;

		OnAccuracyChanged.Broadcast(PlayerStats.Accuracy);
	}

	void IncreaseRounds()
	{
		++this->PlayerStats.Rounds;
	}

	UPROPERTY(BlueprintReadOnly)
	FPlayerStats PlayerStats;

	UPROPERTY(BlueprintAssignable)
	FOnScoreChanged OnScoreChanged;

	UPROPERTY(BlueprintAssignable)
	FOnKillsIncreased OnKillsIncreased;

	UPROPERTY(BlueprintAssignable)
	FOnHeadshotKillsIncreased OnHeadshotKillsIncreased;

	UPROPERTY(BlueprintAssignable)
	FOnAccuracyChanged OnAccuracyChanged;
};
