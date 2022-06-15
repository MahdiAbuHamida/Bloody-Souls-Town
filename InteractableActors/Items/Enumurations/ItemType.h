// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "ItemType.generated.h"


UENUM(Blueprintable)
enum EItemType
{
	Item_Weapon UMETA(DisplayName = "Weapon Item"),
	Item_Quest UMETA(DisplayName = "Quest Item"),
	Item_Ammo UMETA(DisplayName = "Ammo Item")
};