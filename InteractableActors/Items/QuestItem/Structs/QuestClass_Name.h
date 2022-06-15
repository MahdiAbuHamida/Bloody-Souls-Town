#pragma once

#include "QuestClass_Name.generated.h"

USTRUCT( BlueprintType )
struct FQuestClass_Name
{
	GENERATED_USTRUCT_BODY()
	
	FORCEINLINE FQuestClass_Name()
		: QuestItemClass(nullptr), QuestItemName("")
	{
	}
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AQuestItem> QuestItemClass;
	
	UPROPERTY(EditAnywhere)
	FString QuestItemName;
};
