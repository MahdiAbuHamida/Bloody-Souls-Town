// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "QuestData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FQuestData
{
	GENERATED_USTRUCT_BODY()

	friend bool operator==(const FQuestData& Left, const FQuestData& Right)
	{
		return Left.PartsRequired == Right.PartsRequired && Left.QuestId == Right.QuestId;
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 QuestId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 PartsRequired;
};
