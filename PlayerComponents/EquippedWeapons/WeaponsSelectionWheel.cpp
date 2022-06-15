// Mahdi Abu Hamida - Final University Project (2022)


#include "WeaponsSelectionWheel.h"

#include "BloodySoulsTown/Characters/AlexCharacter.h"
#include "BloodySoulsTown/InteractableActors/Items/Item.h"
#include "BloodySoulsTown/Other/MySaveGame.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UWeaponsSelectionWheel::UWeaponsSelectionWheel()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	this->WeaponsArray.SetNum(this->MaximumWeaponsAmount);
}


// Called when the game starts
void UWeaponsSelectionWheel::BeginPlay()
{
	Super::BeginPlay();

	// Save - Load has been cancelled
	this->LoadWeaponWheel();
}

// Called to add a weapon the wheel
void UWeaponsSelectionWheel::AddWeaponToWheel(UWeaponItemObject* InWeaponItemObject)
{ // O(2 * N) N = 0...WeaponsArray.Num()
	if (InWeaponItemObject == nullptr) return;
	
	// Check if the requested weapon is already in the weapons wheel,
	// if true, return false and do not add it again.
	for (const UItemObject* ItemObject : this->WeaponsArray)
	{
		if (ItemObject != nullptr && ItemObject->GetItemClass() == InWeaponItemObject->GetItemClass())
		{
			return;
		}
	}

	AAlexCharacter* PlayerCharacter = Cast<AAlexCharacter>(this->GetOwner());
	if (PlayerCharacter == nullptr) return;

	TArray<UWeaponItemObject*> WeaponSlots = PlayerCharacter->GetWeaponSlots();

	int8 FirstEmptySlot = 0;
	// Finding the first empty weapon slot with the player
	while (FirstEmptySlot < WeaponSlots.Num())
	{
		if (WeaponSlots[FirstEmptySlot] == nullptr) break;
		++FirstEmptySlot;
	}
	
	const bool bHasWeapon = PlayerCharacter->GetCurrentWeapon() != nullptr;
	for (int8 i = 0; i < this->WeaponsArray.Num(); i++)
	{
		if (this->WeaponsArray[i] == nullptr)
		{
			this->WeaponsArray[i] = InWeaponItemObject;
			WeaponSlots[i] = this->WeaponsArray[i];
			PlayerCharacter->SetWeaponSlots(WeaponSlots);
			// If the player has a weapon then return
			if (bHasWeapon) return this->OnAddedWeaponDelegate.Broadcast();
			// Otherwise, make the player to use the right weapon based on the empty slot
			switch (FirstEmptySlot)
			{
				case 0:{ if (!bHasWeapon) { PlayerCharacter->UseWeaponSlot1(); } break; }
				case 1:{ if (!bHasWeapon) { PlayerCharacter->UseWeaponSlot2(); } break; }
				case 2:{ if (!bHasWeapon) { PlayerCharacter->UseWeaponSlot3(); } break; }
				case 3:{ if (!bHasWeapon) { PlayerCharacter->UseWeaponSlot4(); } break; }
				default: ;
			}
			return this->OnAddedWeaponDelegate.Broadcast();
		}
	}
	this->OnNoPlaceForEquippingWeapon.Broadcast();
}

// Called to remove a weapon from the wheel
void UWeaponsSelectionWheel::RemoveWeaponFromWheel(UWeaponItemObject* InWeaponItemObject)
{ // O(N) N = 0...WeaponsArray.Num()
	if (InWeaponItemObject == nullptr) return;
	AAlexCharacter* OwnerCharacter = Cast<AAlexCharacter>(this->GetOwner());
	if (OwnerCharacter == nullptr) return;
	for (uint8 i = 0; i < this->WeaponsArray.Num(); i++)
	{
		if (this->WeaponsArray[i] != nullptr && this->WeaponsArray[i]->GetItemId() == InWeaponItemObject->GetItemId())
		{
			const FName WeaponItemId = this->WeaponsArray[i]->GetItemId();
			this->WeaponsArray[i] = nullptr;
			OwnerCharacter->RemoveWeaponFromSlot(WeaponItemId);
			this->OnRemovedWeaponDel.Broadcast();
			return;
		}
	}
}

// Method returns whether a weapon exists in the wheel or not
bool UWeaponsSelectionWheel::CheckIfWeaponExists(UItemObject* InWeaponItemObject) const
{ // O(1)
	UWeaponItemObject* WeaponItemObject = Cast<UWeaponItemObject>(InWeaponItemObject);
	return WeaponItemObject == nullptr ? false : this->WeaponsArray.Contains(WeaponItemObject);
}

// Save/ load game has been cancelled
// Called to load the saved weapons 
void UWeaponsSelectionWheel::LoadWeaponWheel()
{ // O(N) N = 0...Min(WeaponsArray.Num(), SavedWeapons.Num()) 
	if (!UGameplayStatics::DoesSaveGameExist("MySlot", 0)) return;
	
	const UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot("MySlot", 0));
	if (SaveGameInstance == nullptr) return;
	
	const TArray<UWeaponItemObject*> SavedWeapons = SaveGameInstance->WeaponsInWheel;
	for (int32 i = 0; i < SavedWeapons.Num() && i < this->WeaponsArray.Num(); i++)
	{
		if (SavedWeapons[i] != nullptr)
		{
			this->WeaponsArray[i] = SavedWeapons[i];
		}
	}
}


/* Getters*/

// Method returns all weapons in the wheel
TArray<UWeaponItemObject*> UWeaponsSelectionWheel::GetWeaponsInWheel() const
{ // O(1)
	return this->WeaponsArray;
}
