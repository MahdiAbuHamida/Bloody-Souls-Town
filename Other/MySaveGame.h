// M&M Games 2021 - Free University Project

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/InteractableActors/Items/ItemObjects/AmmoItemObject.h"
#include "BloodySoulsTown/InteractableActors/Items/ItemObjects/WeaponItemObject.h"
#include "BloodySoulsTown/InteractableActors/Items/Weapons/WeaponType.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UMySaveGame()
	{
		
	}
	
	/* The Actors than has been destroyed by the player's actions in the game
	   e.g. the player has the ability to pick up items,
	 		so the item destroyed. and when the game is about to get saved,
	 		these items he got should be gone and destroyed next time the game loads.
	 */
	UPROPERTY()
	TArray<FName> DestroyedActors;

	
	/* Player's properties */
	
	UPROPERTY()
	float PlayerHealth;
	
	// Current weapon class
	UPROPERTY()
	TSubclassOf<AItem> PlayerCurrentWeaponClass;

	// Current item object in use
	UPROPERTY()
	UItemObject* CurrentItemObjectInUse;
	
	// Weapons slots
	UPROPERTY()
	TArray<UWeaponItemObject*> PlayerWeaponsSlots;
	UPROPERTY()
	int32 CurrentWeaponSlotIndex;

	// Power stones
	UPROPERTY()
	TMap<FName, bool> PowerStones;

	
	/* Sensitivities */
	
	/* Controller (Gamepad) */
	
	UPROPERTY(BlueprintReadWrite)
	int32 ControllerSenX;
	UPROPERTY(BlueprintReadWrite)
	int32 ControllerSenY;
	UPROPERTY(BlueprintReadWrite)
	float LowZoomAdsControllerSenModifier;
	UPROPERTY(BlueprintReadWrite)
	float HighZoomAdsControllerSenModifier;
	UPROPERTY(BlueprintReadWrite)
	float RightStickDeadZone;
	UPROPERTY(BlueprintReadWrite)
	float LeftStickDeadZone;

	
	/* Mouse */
	
	UPROPERTY(BlueprintReadWrite)
	float MouseSenX;
	UPROPERTY(BlueprintReadWrite)
	float MouseSenY;
	UPROPERTY(BlueprintReadWrite)
	float LowZoomAdsMouseSenModifier;
	UPROPERTY(BlueprintReadWrite)
	float HighZoomAdsMouseSenModifier;


	/* Audio */
	UPROPERTY(BlueprintReadWrite)
	float MasterAudioClassVolume;
	UPROPERTY(BlueprintReadWrite)
	float MusicAudioClassVolume;
	UPROPERTY(BlueprintReadWrite)
	float EffectsAudioClassVolume;
	UPROPERTY(BlueprintReadWrite)
	float VoiceAudioClassVolume;

	
	/* Player's Inventory Items */
	
	UPROPERTY()
	TArray<UItemObject*> InvItems;
	UPROPERTY()
	TMap<FName, int32> InvAmmoItemObjectsAmmoAmount;
	UPROPERTY()
	TMap<FName, int32> InvGunsItemObjectsCurrentAmmoAmount;
	UPROPERTY()
	TMap<FName, int32> InvRightGunsItemObjectsCurrentAmmoAmount;

	// Player's Weapons In Wheel (Weapon Slots)
	UPROPERTY()
	TArray<UWeaponItemObject*> WeaponsInWheel;

	
	/* Interactable actors in the main gameplay level*/
	
	UPROPERTY()
	TMap<FName, FRotator> InteractableActorsRotations;
	UPROPERTY()
	TMap<FName, FVector> ItemsLocations;
	UPROPERTY()
	TMap<FName, int32> GunsCurrentAmmo;
	UPROPERTY()
	TMap<FName, int32> RightGunsCurrentAmmo;
	UPROPERTY()
	TMap<FName, int32> AmmoItemsCurrentAmmo;
};