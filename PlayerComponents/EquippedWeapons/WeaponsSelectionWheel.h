// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/InteractableActors/Items/ItemObjects/WeaponItemObject.h"
#include "Components/ActorComponent.h"

#include "WeaponsSelectionWheel.generated.h"


/* Delegate Dispatchers Declaration */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAddedWeaponDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRemovedWeaponDel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNoPlaceForEquippingWeapon);


/******************************************************************************************/


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLOODYSOULSTOWN_API UWeaponsSelectionWheel final : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponsSelectionWheel();
	
	
	/* Main methods */

	// Called to add a weapon the wheel
	UFUNCTION(BlueprintCallable)
	void AddWeaponToWheel(UWeaponItemObject* InWeaponItemObject);

	// Called to remove a weapon from the wheel
	UFUNCTION(BlueprintCallable)
	void RemoveWeaponFromWheel(UWeaponItemObject* InWeaponItemObject);

	// Method returns whether a weapon exists in the wheel or not
	bool CheckIfWeaponExists(UItemObject* InWeaponItemObject) const;

	// Save/ load game has been cancelled
	// Called to load the saved weapons 
	void LoadWeaponWheel();

	
	/* Getters*/

	// Method returns all weapons in the wheel
	UFUNCTION(BlueprintCallable)
	TArray<UWeaponItemObject*> GetWeaponsInWheel() const;


	/* Event dispatcher delegates */

	// When a weapon is added to the weapons wheel
	UPROPERTY(BlueprintAssignable)
	FOnAddedWeaponDelegate OnAddedWeaponDelegate;

	// When there's no place to add a weapon
	UPROPERTY(BlueprintAssignable)
	FOnNoPlaceForEquippingWeapon OnNoPlaceForEquippingWeapon;

	// When a weapon is removed from the wheel
	UPROPERTY(BlueprintAssignable)
	FOnRemovedWeaponDel OnRemovedWeaponDel;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Array of weapons item objects
	UPROPERTY()
	TArray<UWeaponItemObject*> WeaponsArray;

	// The maximum number of weapons in the wheel
	const int32 MaximumWeaponsAmount = 4;
};