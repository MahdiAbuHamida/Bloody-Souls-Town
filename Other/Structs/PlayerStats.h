// M&M Games 2021 - Free University Project

#pragma once

#include "PlayerStats.generated.h"

USTRUCT(BlueprintType)
struct FPlayerStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 Rounds;

	UPROPERTY(BlueprintReadOnly)
	int32 Score;

	UPROPERTY(BlueprintReadOnly)
	int32 Kills;

	UPROPERTY(BlueprintReadOnly)
	int32 HeadshotKills;

	UPROPERTY(BlueprintReadOnly)
	int32 Accuracy;
	
	int32 ShotsFired;
	
	int32 ShotsLanded;
};
