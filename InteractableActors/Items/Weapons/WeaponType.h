// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "WeaponType.generated.h"

UENUM(Blueprintable)
enum EWeaponType
{
	Weapon_Melee UMETA(DisplayName = "Melee"),
	Weapon_Pistol_Smg UMETA(DisplayName = "Pistol_Smg"),
	Weapon_Shotgun UMETA(DisplayName = "Shotgun"),
	Weapon_GrenadeLauncher UMETA(DisplayName = "Grenade Launcher"),
	Weapon_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	Weapon_Sniper UMETA(DisplayName = "Sniper")
};