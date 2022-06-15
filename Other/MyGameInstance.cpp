// M&M Games 2021 - Free University Project


#include "MyGameInstance.h"

#include "BloodySoulsTown/Characters/AlexCharacter.h"
#include "MySaveGame.h"
#include "BloodySoulsTown/PlayerComponents/Inventory/Inventory.h"
#include "BloodySoulsTown/PlayerComponents/EquippedWeapons/WeaponsSelectionWheel.h"
#include "BloodySoulsTown/InteractableActors/Items/AmmoItem/AmmoItem.h"
#include "BloodySoulsTown/InteractableActors/Items/Weapons/Gun.h"
#include "Kismet/GameplayStatics.h"

UMyGameInstance::UMyGameInstance()
{
	if (UGameplayStatics::DoesSaveGameExist("MySlot", 0))
	{
		const UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot("MySlot", 0));
		this->DestroyedActors = SaveGameInstance->DestroyedActors;
	}
}

void UMyGameInstance::DestroyItem(const FName ItemID)
{
	this->DestroyedActors.AddUnique(ItemID);
}

bool UMyGameInstance::CheckIfAlreadyDestroyed(const FName ItemID) const
{
	return this->DestroyedActors.Find(ItemID) > -1;
}

void UMyGameInstance::SaveGame()
{
	AAlexCharacter* PlayerCharacter = Cast<AAlexCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (PlayerCharacter == nullptr) return;
	// Create an instance of save game class
	UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(
	UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass())
	);
		
	/*
	* Getting properties to be saved
	*/
	
	// Saving the destroyed actors in the current map
	SaveGameInstance->DestroyedActors = this->DestroyedActors;
	const UWorld* World = this->GetWorld();
	// Saving the Interactable actors rotations & Items location in the current map
	UpdateItemsLocationsRotations(World);
	SaveGameInstance->ItemsLocations = ItemsLocations;
	SaveGameInstance->InteractableActorsRotations = InteractableActorsRotations;
	// Saving guns ammo amounts in the map
	UpdateGunsCurrentAmmoAmount(World);
	SaveGameInstance->GunsCurrentAmmo = GunsCurrentAmmoAmount;
	SaveGameInstance->RightGunsCurrentAmmo = RightGunsCurrentAmmoAmount;
	// Saving player's inventory items
	UInventory* PlayerInventory = PlayerCharacter->GetPlayerInventory();
	if (PlayerInventory != nullptr)
	{
		SaveGameInstance->InvItems = PlayerInventory->GetItems();
		PlayerInventory->UpdateItemObjectsAmmoAmounts();
		SaveGameInstance->InvAmmoItemObjectsAmmoAmount = PlayerInventory->AmmoItemObjectsAmmoAmountPerItemId;
		SaveGameInstance->InvRightGunsItemObjectsCurrentAmmoAmount = PlayerInventory->RightGunsItemObjectsAmmoAmountPerItemId;
	}
	// Saving the weapons from the weapon selection wheel
	UWeaponsSelectionWheel* WeaponSelectionWheel = PlayerCharacter->GetWeaponsWheel();
	if (WeaponSelectionWheel != nullptr)
	{
		SaveGameInstance->WeaponsInWheel = WeaponSelectionWheel->GetWeaponsInWheel();
		// Saving the player's weapon slots
		SaveGameInstance->PlayerWeaponsSlots = PlayerCharacter->GetWeaponSlots();
		SaveGameInstance->CurrentWeaponSlotIndex = PlayerCharacter->CurrentWeaponSlotIndex;
		if (UItemObject* ItemObject = PlayerCharacter->GetCurrentItemObjectInUse())
		{
			SaveGameInstance->PlayerCurrentWeaponClass = ItemObject->GetItemClass();
			SaveGameInstance->CurrentItemObjectInUse = ItemObject;
		}
	}
	
	// Saving the player's properties
	SaveGameInstance->PlayerHealth = PlayerCharacter->CurrentHealth;

	// Saving the player's power stones
	TArray<EPowerStoneType> PowerStones = PlayerCharacter->GetPlayerPowerStonesByTypes();
	
	SaveGameInstance->PowerStones.Add("Speedy", false);
	SaveGameInstance->PowerStones.Add("DoubleSouls", false);
	SaveGameInstance->PowerStones.Add("RapidFire", false);
	SaveGameInstance->PowerStones.Add("BigShoulders", false);

	for (const EPowerStoneType PowerStone : PowerStones)
	{
		switch (PowerStone)
		{
			case PowerStone_Speedy: {SaveGameInstance->PowerStones["Speedy"] = true; break;}
			case PowerStone_DoubleSouls: {SaveGameInstance->PowerStones["DoubleSouls"] = true; break;}
			case PowerStone_RapidFire: 	{SaveGameInstance->PowerStones["RapidFire"] = true; break;}
			case PowerStone_BigShoulders: {SaveGameInstance->PowerStones["BigShoulders"] = true; break;}
			default: ;
		}
	}
	
	/*
	 * Saving the game to slot
	*/
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("MySlot"), 0);
}

// Method to get all items locations in the current game world
void UMyGameInstance::UpdateItemsLocationsRotations(const UWorld* World)
{
	TArray<AActor*> InteractableActorsFound;
	UGameplayStatics::GetAllActorsOfClass(World, AInteractable::StaticClass(), InteractableActorsFound);
	for (AActor* CurrentActor : InteractableActorsFound)
	{
		if (AInteractable* CurrentInteractableActor = Cast<AInteractable>(CurrentActor))
		{
			// later
			// this->InteractableActorsRotations.Add();
			AItem* CurrentItem = Cast<AItem>(CurrentInteractableActor);
			if (CurrentItem != nullptr)
			{
				ItemsLocations.Add(CurrentItem->GetItemId(), CurrentItem->GetActorLocation());
				InteractableActorsRotations.Add(CurrentItem->GetItemId(), CurrentItem->GetActorRotation());
			}
		}
	}
}

void UMyGameInstance::UpdateGunsCurrentAmmoAmount(const UWorld* World)
{
	TArray<AActor*> GunsItemsFound;
	UGameplayStatics::GetAllActorsOfClass(World, AGun::StaticClass(), GunsItemsFound);
	for (AActor* CurrentActor : GunsItemsFound)
	{
		if (const AGun* CurrentGun = Cast<AGun>(CurrentActor))
		{
			GunsCurrentAmmoAmount.Add(CurrentGun->GetItemId(), CurrentGun->GetCurrentAmmo());
			if (const UWeaponItemObject* WeaponItemObject = Cast<UWeaponItemObject>(CurrentGun->GetItemObject()))
			{
				RightGunsCurrentAmmoAmount.Add(CurrentGun->GetItemId(), WeaponItemObject->RightHandGunCurrentAmmo);
			}
		}
	}
	
}

TMap<FName, int32> UMyGameInstance::GetAmmoItemsAmmoAmount() const
{
	TMap<FName, int32> AmmoItemsAmmoAmounts;
	TArray<AActor*> AmmoItemsFound;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AAmmoItem::StaticClass(), AmmoItemsFound);
	for (AActor* CurrentActor : AmmoItemsFound)
	{
		if (const AAmmoItem* CurrentAmmoItem = Cast<AAmmoItem>(CurrentActor))
		{
			if (const UAmmoItemObject* AmmoItemObject = Cast<UAmmoItemObject>(CurrentAmmoItem->GetItemObject()))
			{
				AmmoItemsAmmoAmounts.Add(CurrentAmmoItem->GetItemId(), AmmoItemObject->CurrentTotalAmmo);
			}
		}
	}
	return AmmoItemsAmmoAmounts;
}
