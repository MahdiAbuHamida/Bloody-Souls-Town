// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "QuestType.generated.h"

UENUM(Blueprintable)
enum EQuestType
{
	Shield UMETA (DisplayName = "Shield"),
	ShieldPart UMETA (DisplayName = "ShieldPart"),
	Key UMETA (DisplayName = "Key"),
	KeyPart UMETA (DisplayName = "KeyPart")
};
