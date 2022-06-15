// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BloodySoulsTown/InteractableActors/Items/ItemObjects/ItemObject.h"
#include "Structs/TileGrid.h"
#include "BloodySoulsTown/InteractableActors/Items/Weapons/WeaponType.h"
#include "Inventory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChangedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRowsColumnsChangedDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class BLOODYSOULSTOWN_API UInventory final : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventory();

	// Called to update the items array after rows/cols change
	UFUNCTION(BlueprintCallable)
	void RefreshInventoryAfterColsRowsChange();

	
	/* Getters */

	// Method returns all items in the inventory with tile grid for each item
	UFUNCTION(BlueprintPure)
	TMap<UItemObject* , FTileGrid> GetAllItems() const;

	// Method returns all items in the inventory
	UFUNCTION(BlueprintCallable)
	TArray<UItemObject*> GetItems() const;

	// Method returns ammo amount in the inventory of a given single ammo type
	UFUNCTION(BlueprintPure)
	int32 GetAmmoByWeaponType(const EWeaponType InWeaponType) const;

	// Method returns all ammo items in the inventory
	TArray<class UAmmoItemObject*> GetAmmoItemObjects();

	// Method returns all ammo items of a single type in the inventory
	TArray<UAmmoItemObject*> GetAmmoItemObjectsOfType(const EWeaponType AmmoType);

	
	/* Main methods */

	/*
	  Called to add an item at a given index.
	  starts from top left, to the right (item x size)
	  and from top to down (item y size)
	 */
	UFUNCTION(BlueprintCallable)
	void AddItemAt(UItemObject* InItemObject, const int32 TopLeftIndex, const bool bAddItemToPlayer);

	/*
	  Called to try to add a given item to the inventory
	  Looks for an empty space inside the inventory, if found, calls "AddItemAt" function.
	 */
	UFUNCTION(BlueprintCallable)
	bool TryAddItem(UItemObject* InItemObject, const bool bAddItemToPlayer);

	/*
	  Called to check if there's an empty space for the given item.
	  Looks at the given top left index to the item's x & y sizes
	 */
	UFUNCTION(BlueprintCallable)
	bool IsRoomAvailable(UItemObject* InItemObject, const int32 TopLeftIndex);

	// Called to remove a given item from the inventory 
	UFUNCTION(BlueprintCallable)
	void RemoveItem(UItemObject* InItemObject, const bool bRemoveFromWeaponsWheel, const bool bRelease);

	// Called to remove actual weapon from the player or weapons wheel
	UFUNCTION(BlueprintCallable)
	void RemoveItemFromPlayer(UItemObject* InItemObject, const bool bRemoveFromWeaponsWheel, const bool bRelease) const;

	// Convert a given Tile Grid to an index
	UFUNCTION(BlueprintCallable)
	int32 TileToIndex(const FTileGrid TileGrid) const;

	/*
	  Called to update ammo amount in each ammo item or a gun
	  this method used for saving the inventory ammo related things
	  but save game has been cancelled
	 */
	void UpdateItemObjectsAmmoAmounts();

	// Called to update the ammo amounts map in the inventory
	void UpdateAmmoAmounts();

	// Called to decrease ammo amount of a weapon type (ammo type) with a given amount to decrease
	void DecreaseAmmoByAmmoType(const EWeaponType InWeaponType, int32 InAmmoAmount);

	// Called to check if removing an item can give a room for new item
	bool TestRemoveItemCheckRoomForNewItem(UItemObject* InItemObjectToRemove, UItemObject* InItemObjectToAdd);

	
	/* Gamepad navigation methods */

	/*
	  Called to find the next item of a given item, when the user hits right arrow/ stick
	  using a gamepad device
	 */
	UFUNCTION(BlueprintCallable)
	UItemObject* NextItemRightNavigation(UItemObject* InItemObject);

	/*
	  Called to find the next item of a given item, when the user hits left arrow/ stick
	  using a gamepad device
	*/
	UFUNCTION(BlueprintCallable)
	UItemObject* NextItemLeftNavigation(UItemObject* InItemObject);

	/*
	  Called to find the next item of a given item, when the user hits up arrow/ stick
	  using a gamepad device
	 */
	UFUNCTION(BlueprintCallable)
	UItemObject* NextItemUpNavigation(UItemObject* InItemObject);

	/*
	  Called to find the next item of a given item, when the user hits down arrow/ stick
	  using a gamepad device
	 */
	UFUNCTION(BlueprintCallable)
	UItemObject* NextItemDownNavigation(UItemObject* InItemObject);

	
	/* Public Properties */

	// Number of the columns of the inventory
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cols-Rows")
	int32 Columns = 10;

	// Number of the rows of the inventory
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cols-Rows")
	int32 Rows = 6;

	
	/* Events Dispatchers */

	// Notify that a change happened in the inventory
	UPROPERTY(BlueprintAssignable)
	FOnInventoryChangedDelegate OnInventoryChangedDel;

	// Notify that the columns or the rows number has been changed 
	UPROPERTY(BlueprintAssignable)
	FOnRowsColumnsChangedDelegate OnRowsColumnsChangedDel;

	
	/* Public properties used for saving ammo items in depth data */
	
	TMap<FName, int32> AmmoItemObjectsAmmoAmountPerItemId;
	TMap<FName, int32> GunsItemObjectsCurrentAmmoAmountPerItemId;
	TMap<FName, int32> RightGunsItemObjectsAmmoAmountPerItemId;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Called to initialize the inventory
	void InitializeInventory();

	/* Save game has been cancelled */
	// Called to load from save game
	void LoadInventory();

	/*
	  Converts a given index to a Tile Gird
	  Tile Grid: a point of 2 values (x,y)
	  x: the x value in a grid (given n: x = n % columns)
	  y: the y value in a grid (given z: y = z / columns)
	 */
	FTileGrid IndexToTile(const int32 Index) const;

	// Array of items (simple objects - not 3d items-actors)
	UPROPERTY()
	TArray<UItemObject*> Items;

	// Map of a weapon types (ammo types) key connected to the amounts values
	TMap<TEnumAsByte<EWeaponType>, int32> AmmoAmountsMap;
};
