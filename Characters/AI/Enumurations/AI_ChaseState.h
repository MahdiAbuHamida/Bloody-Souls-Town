// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "AI_ChaseState.generated.h"

UENUM(BlueprintType)
enum EAI_ChaseState
{
	AI_ChaseState_Idle UMETA(DisplayName = "Idle"),
	AI_ChaseState_ChasingPlayer UMETA(DisplayName = "ChasingPlayer")
};

