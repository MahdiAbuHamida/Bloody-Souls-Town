// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "InteractableActorType.generated.h"

UENUM()
enum EInteractableActorType
{
	Interactable_Item UMETA(DisplayName = "Item"),
	Interactable_Door UMETA(DisplayName = "Door"),
	Interactable_MysteryBox UMETA(DisplayName = "MysteryBox"),
	Interactable_WallWeapon UMETA(DisplayName = "WallWeapon"),
	Interactable_PowerStone UMETA(DisplayName = "PowerStone"),
	Interactable_Cash UMETA(DisplayName = "Cash"),
	Interactable_Obstacle UMETA(DisplayName = "Obstacle"),
	Interactable_PartsBuildingTable UMETA(DisplaName = "PartsBuildingTable")
};