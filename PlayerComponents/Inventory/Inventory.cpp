// Mahdi Abu Hamida - Final University Project (2022)


#include "Inventory.h"
#include "BloodySoulsTown/Characters/AlexCharacter.h"
#include "BloodySoulsTown/PlayerComponents/EquippedWeapons/WeaponsSelectionWheel.h"
#include "BloodySoulsTown/InteractableActors/Items/Weapons/Gun.h"
#include "BloodySoulsTown/Other/MySaveGame.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Method returns ammo amount in the inventory of a given single ammo type
int32 UInventory::GetAmmoByWeaponType(const EWeaponType InWeaponType) const
{ // O(1)
	return this->AmmoAmountsMap.Contains(InWeaponType) ? this->AmmoAmountsMap[InWeaponType] : 0;
}

// Method returns all ammo items in the inventory
TArray<UAmmoItemObject*> UInventory::GetAmmoItemObjects()
{ // O(N) : N = 0...Items.Num()
	TArray<UAmmoItemObject*> AmmoItemObjects;
	for (UItemObject* ItemObject : this->Items)
	{
		// Check if it is an ammo item object
		if (UAmmoItemObject* AmmoItemObject = Cast<UAmmoItemObject>(ItemObject))
		{
			// Add it to the array
			AmmoItemObjects.AddUnique(AmmoItemObject);
		}
	}
	return AmmoItemObjects;
}

// Method returns all ammo items of a single type in the inventory
TArray<UAmmoItemObject*> UInventory::GetAmmoItemObjectsOfType(const EWeaponType AmmoType)
{ // O(N) : N = 0...Items.Num()
	TArray<UAmmoItemObject*> AmmoItemObjectsOfType;
	for (UItemObject* ItemObject : this->Items)
	{
		UAmmoItemObject* AmmoItemObject = Cast<UAmmoItemObject>(ItemObject);
		// Check if it is an ammo item & that ammo item ammo type is of the same given same
		if (AmmoItemObject != nullptr && AmmoItemObject->AmmoTypeByWeapon == AmmoType)
		{
			// Add it to the array
			AmmoItemObjectsOfType.AddUnique(AmmoItemObject);
		}
	}
	return AmmoItemObjectsOfType;
}

/*
  Called to update ammo amount in each ammo item or a gun
  this method used for saving the inventory ammo related things
  but save game has been cancelled
 */
void UInventory::UpdateItemObjectsAmmoAmounts()
{ // O(N) : N = 0...Items.Num()
	int32 i = 0;
	while (i < this->Items.Num())
	{
		if (this->Items[i] != nullptr)
		{
			// check if it's an ammo item object to save ammo amount it holds
			if (const UAmmoItemObject* AmmoItemObject = Cast<UAmmoItemObject>(this->Items[i]))
			{
				this->AmmoItemObjectsAmmoAmountPerItemId.Add(AmmoItemObject->GetItemId(), AmmoItemObject->CurrentTotalAmmo);
				// Ignore the same item
				i += AmmoItemObject->GetDimensions().X;
			}
			// Otherwise check if it's a weapon (gun) item object to save ammo amount the gun has in mag
			else if (const UWeaponItemObject* WeaponItemObject = Cast<UWeaponItemObject>(this->Items[i]))
			{
				if (WeaponItemObject->IsGun)
				{
					this->GunsItemObjectsCurrentAmmoAmountPerItemId.Add(WeaponItemObject->GetItemId(),
						WeaponItemObject->GunObjectCurrentAmmo);
					this->RightGunsItemObjectsAmmoAmountPerItemId.Add(WeaponItemObject->GetItemId(),
						WeaponItemObject->RightHandGunCurrentAmmo);
					// Ignore the same item
					i += WeaponItemObject->GetDimensions().X;
				}
			}
		}
		// Otherwise can't get dimensions
		else
		{
			i++;
		}
	}
}

// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay();

	// Called to initialize the inventory
	this->InitializeInventory();
}

// Called to refresh the items objects array once the columns or rows number change
void UInventory::RefreshInventoryAfterColsRowsChange()
{ // O(N) : N = Items.Num()
	TArray<UItemObject*> NewItems;
	NewItems.SetNum(this->Columns * this->Rows);
	
	for (int32 i = 0; i < NewItems.Num() && i < this->Items.Num(); i++)
	{
		NewItems[i] = this->Items[i];
	}

	this->Items = NewItems;
}


/* Getters */

// Method returns all items in the inventory with tile grid for each item
TMap<UItemObject*, FTileGrid> UInventory::GetAllItems() const
{ // O(N) : N = 0...Items.Num()
	TMap<UItemObject*, FTileGrid> AllItems;
	int32 i = 0;
	do
	{
		// Check if item is valid
		if (this->Items[i] != nullptr)
		{
			// If the array doesn't contain that item
			if (!AllItems.Contains(this->Items[i]))
			{
				// Add it to the array
				AllItems.Add(this->Items[i], IndexToTile(i));
			}
			// Ignore the same item
			i += this->Items[i]->GetDimensions().X;
		}
		// Otherwise can't get dimensions
		else
		{
			i++;
		}
	} while (i < this->Items.Num());
	
	return AllItems;
}

// Method returns all items in the inventory
TArray<UItemObject*> UInventory::GetItems() const
{ // O(1)
	return this->Items;
}


/* Main methods */

/*
  Called to add an item at a given index.
  starts from top left, to the right (item x size)
  and from top to down (item y size)
 */
void UInventory::AddItemAt(UItemObject* InItemObject, const int32 TopLeftIndex, const bool bAddItemToPlayer)
{ // O(N * M + K) : N = TopLeftIndex...LastIndexH (right), M = TopLeftIndex...LastIndexV (down),
	// K = 0...Weapons.Num() => WeaponsWheel->AddWeaponToWheel()
	
	// Convert the index to a tile (x=idx%columns, y=idx/columns)
	const FTileGrid TileGrid = IndexToTile(TopLeftIndex);
	// Validate the tile grid values
	if (TileGrid.X == -1 || TileGrid.Y == -1)
	{
		return;
	}
	
	// Assigning x, y values equal to item x, y dimensions
	const FIntPoint Dimensions = InItemObject->GetDimensions();

	// e.g. item exists in indexes 5-7 & 10-12 (dX = 3, dY = 2), (tileX = TopLeftIndex % Columns = 5 % 5 = 0 (c0)
	// , tileY = TopLeftIndex / columns = 5 / 5 = 1 (r1))
	
	//    c0 c1 c2 c3 c4
	// r0:[x][x][x][x][x]
	// r1:[5][6][7][x][x]
	//r2:[10][11][12][x][x]

	// Starts at tileX (e.g. 0), ends at tileX + (dX - 1) (e.g. 2)  =>  N
	const int32 LastIndexH = TileGrid.X + (Dimensions.X - 1);
	// Starts at tileY (e.g. 1), ends at tileY + (dY - 1) (e.g. 2 (r2))  =>  M
	const int32 LastIndexV = TileGrid.Y + (Dimensions.Y - 1);
	// N
	for (int32 HorizontalIndex = TileGrid.X; HorizontalIndex <= LastIndexH; HorizontalIndex++)
	{
		FTileGrid TileGridAdd;
		// Tile x = HorizontalIndex (e.g. 0 (first iteration))
		TileGridAdd.X = HorizontalIndex;
		// M
		for (int32 VerticalIndex = TileGrid.Y; VerticalIndex <= LastIndexV; VerticalIndex++)
		{
			// Tile y = VerticalIndex (e.g.  (first iteration))
			TileGridAdd.Y = VerticalIndex;
			// (e.g. x = 0, y = 1 (first iteration) => index = x + (y * columns) = 0 + (1 * 5) = 5)
			Items[TileToIndex(TileGridAdd)] = InItemObject;
		}
	}
	// Called to refresh inventory items widget
	this->OnInventoryChangedDel.Broadcast();

	// Check if need to add item to the player (to the weapons wheel component)
	if (!bAddItemToPlayer) return;
	
	const AAlexCharacter* PlayerCharacter = Cast<AAlexCharacter>(this->GetOwner());
	UWeaponsSelectionWheel* WeaponsWheel = PlayerCharacter->GetWeaponsWheel();
	UWeaponItemObject* WeaponItemObject = Cast<UWeaponItemObject>(InItemObject);
	
	if (WeaponsWheel != nullptr && WeaponItemObject != nullptr)
	{
		WeaponsWheel->AddWeaponToWheel(WeaponItemObject);
	}
}

/*
  Called to try to add a given item to the inventory
  Looks for an empty space inside the inventory, if found, calls "AddItemAt" function
  and returns true, otherwise returns false
 */
bool UInventory::TryAddItem(UItemObject* InItemObject, const bool bAddItemToPlayer)
{ // O(N * (K * M)^2 + U) : 0...(Items.Num() * (TopLeftIndex...LastIndexH, TopLeftIndex...[TopLeftIndex*LastIndexV]^2 =>
	// IsRoomAvailable() + Weapons.Num() => WeaponsWheel->AddWeaponToWheel()))
	if (InItemObject == nullptr) return false;
	
	for (int32 TopLeftIndex = 0; TopLeftIndex < Items.Num(); TopLeftIndex++)
	{
		// Check if there's a room to add this item
		if (IsRoomAvailable(InItemObject, TopLeftIndex))
		{
			// Add the item to the inventory at the available index/es
			AddItemAt(InItemObject, TopLeftIndex, bAddItemToPlayer);
			return true;
		}
	}
	
	const FIntPoint ItemDimensions = InItemObject->GetDimensions();
	/*
	  Before trying to add the item by rotating it, check if dim x & y are not equal
	  cause otherwise it'll be like a square shape, rotating it doesn't make any change
	 */
	if (ItemDimensions.X == ItemDimensions.Y) return false;

	// Rotating the item icon (x = y, y = x)
	InItemObject->RotateIcon();

	// Iterating again but with the icon rotated
	for (int8 TopLeftIndex = 0; TopLeftIndex < Items.Num(); TopLeftIndex++)
	{
		if (IsRoomAvailable(InItemObject, TopLeftIndex))
		{
			AddItemAt(InItemObject, TopLeftIndex, bAddItemToPlayer);
			return true;
		}
	}
	// No room is available, return false
	return false;
}

/*
  Called to check if there's an empty space for the given item.
  Looks at the given top left index to the item's x & y sizes
 */
bool UInventory::IsRoomAvailable(UItemObject* InItemObject, const int32 TopLeftIndex)
{ // O(N * M) : N = TopLeftIndex...LastIndexH (right), M = TopLeftIndex...LastIndexV (down)
	const FTileGrid TileGrid = IndexToTile(TopLeftIndex);
	// Validate the tile grid
	if (TileGrid.X == -1 || TileGrid.Y == -1) return false;

	const FIntPoint Dimensions = InItemObject->GetDimensions();

	// e.g. item exists in indexes 5-7 & 10-12 (dX = 3, dY = 2), (tileX = TopLeftIndex % Columns = 5 % 5 = 0 (c0)
	// , tileY = TopLeftIndex / columns = 5 / 5 = 1 (r1))
	
	//    c0 c1 c2 c3 c4
	// r0:[x][x][x][x][x]
	// r1:[5][6][7][x][x]
	//r2:[10][11][12][x][x]

	// Starts at tileX (e.g. 0), ends at tileX + (dX - 1) (e.g. 2)  =>  N
	const int32 LastIndexH = TileGrid.X + (Dimensions.X - 1);
	// Starts at tileY (e.g. 1), ends at tileY + (dY - 1) (e.g. 2 (r2))  =>  M
	const int32 LastIndexV = TileGrid.Y + (Dimensions.Y - 1);

	// N
	for (int32 HorizontalIndex = TileGrid.X; HorizontalIndex <= LastIndexH; HorizontalIndex++)
	{
		// M
		for (int32 VerticalIndex = TileGrid.Y; VerticalIndex <= LastIndexV; VerticalIndex++)
		{
			FTileGrid TileGridToCheck;
			TileGridToCheck.X = HorizontalIndex;
			TileGridToCheck.Y = VerticalIndex;
			// Validating the tile
			if (TileGridToCheck.X < 0 || TileGridToCheck.Y < 0 || TileGridToCheck.X >= this->Columns
				|| TileGridToCheck.Y >= this->Rows)
			{
				return false;
			}
			const int32 IndexToCheck = TileToIndex(TileGridToCheck);
			// Validating the index & check if an item exists in this tile
			if (IndexToCheck < 0 || IndexToCheck >= Items.Num() || this->Items[IndexToCheck] != nullptr)
			{
				return false;
			}
		}
	}
	// There's a room for the item
	return true;
}

// Called to remove a given item from the inventory
void UInventory::RemoveItem(UItemObject* InItemObject, const bool bRemoveFromWeaponsWheel, const bool bRelease)
{ // O(N) : N = Items.Num()
	if (InItemObject == nullptr) return;
	
	for (int32 i = 0; i < this->Items.Num(); i++)
	{
		if (this->Items[i] != nullptr && this->Items[i]->GetItemId() == InItemObject->GetItemId())
		{
			// Erasing the item
			this->Items[i] = nullptr;
			// Called to refresh inventory items widget
			this->OnInventoryChangedDel.Broadcast();
		}
	}
	
	AAlexCharacter* PlayerCharacter = Cast<AAlexCharacter>(GetOwner());
	// Check if removed item is an ammo
	if (InItemObject->GetItemType() == Item_Ammo)
	{
		// Called to update ammo amounts in the inventory
		this->UpdateAmmoAmounts();
		if (PlayerCharacter != nullptr)
		{
			// Check if the player has a gun
			if (AGun* Gun = Cast<AGun>(PlayerCharacter->GetCurrentWeapon()))
			{
				const UAmmoItemObject* AmmoItemObject = Cast<UAmmoItemObject>(InItemObject);
				// Check if player's gun ammo is the same as the removed ammo item -ammo type
				if (AmmoItemObject != nullptr && Gun->GetWeaponType() == AmmoItemObject->AmmoTypeByWeapon)
				{
					// Reassign total ammo for the gun with the new total in the inventory of this ammo type
					Gun->SetCurrentTotalAmmo(this->GetAmmoByWeaponType(Gun->GetWeaponType()));
				}
			}
		}
	}
	/*
	  If requested to completely remove the item from the player,
	  then called to remove it, cause like in the drag & drop operation
	  the item object gets removed from the inventory (so player can put it again at an index/es of any the same)
	  meaning like a collided tiles yet of same item, so that's why the item gets deleted
	  BUT, the item cannot be removed from the weapons wheel or from the player hands
	  if it was, in drag & drop operation,
	  so passing in false to this function will ignore the function call below ...
	 */
	this->RemoveItemFromPlayer(InItemObject, bRemoveFromWeaponsWheel, bRelease);
}

// Called to remove actual weapon from the player or weapons wheel
void UInventory::RemoveItemFromPlayer(UItemObject* InItemObject, const bool bRemoveFromWeaponsWheel, const bool bRelease) const
{ // O(N) : N = 0...Weapons.Num()
	// At least one condition has to be true
	if (!(bRemoveFromWeaponsWheel || bRelease)) return;
	AAlexCharacter* PlayerCharacter = Cast<AAlexCharacter>(GetOwner());
	if (PlayerCharacter == nullptr) return;
	if (InItemObject->GetItemType() == Item_Weapon)
	{
		if (PlayerCharacter == nullptr) return;
		// Check if has to release weapon
		if (PlayerCharacter->GetCurrentWeapon() != nullptr && bRelease)
		{
			// Release the weapon form player hands
			PlayerCharacter->ReleaseWeapon();
		}
		// If not to be removed from weapons wheel then return
		if (!bRemoveFromWeaponsWheel) return;
		
		UWeaponsSelectionWheel* WeaponsWheel = PlayerCharacter->GetWeaponsWheel();
		UWeaponItemObject* WeaponItemObject = Cast<UWeaponItemObject>(InItemObject);
		if (WeaponsWheel != nullptr && WeaponItemObject != nullptr)
		{
			// Removing the weapon from the weapons wheel
			WeaponsWheel->RemoveWeaponFromWheel(WeaponItemObject);
		}
	}
}

/*
  Converts a given index to a Tile Gird
  Tile Grid: a point of 2 values (x,y)
  x: the x value in a grid (given n: x = n % columns)
  y: the y value in a grid (given n: y = n / columns)
 */
FTileGrid UInventory::IndexToTile(const int32 Index) const
{ // O(1)
	// Check first if columns equal to 0
	if (this->Columns == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Inventory.cpp IndexToTile() says: columns = 0, can't / or % by 0!"));
		// return an invalid tile values
		return FTileGrid(-1, -1);
	}
	return FTileGrid(Index % Columns, Index / Columns);
}

// Convert a given Tile Grid to an index
int32 UInventory::TileToIndex(const FTileGrid TileGrid) const
{ // O(1)
	/*
	  X = column#, Y = row# .. of a tile
	  means row * (inventory columns) gives us the first index of row=(Y)
	  then we add to it the X value which is the column number of the tile
	  that way we get the index
	 */
	return TileGrid.X + TileGrid.Y * Columns;
}

// Called to update the ammo amounts map in the inventory
void UInventory::UpdateAmmoAmounts()
{ // O(2 * N) : N = 0...this->Items.Num() * 2
	// Reassign all ammo amounts for each ammo type to 0
	this->AmmoAmountsMap[Weapon_Pistol_Smg] = 0;
	this->AmmoAmountsMap[Weapon_AssaultRifle] = 0;
	this->AmmoAmountsMap[Weapon_Shotgun] = 0;
	this->AmmoAmountsMap[Weapon_Sniper] = 0;
	this->AmmoAmountsMap[Weapon_GrenadeLauncher] = 0;
	
	for (UItemObject* ItemObject : this->Items)
	{
		// Check if item is an ammo item
		if (const UAmmoItemObject* AmmoItemObject = Cast<UAmmoItemObject>(ItemObject))
		{
			// Adding ammo amount of this ammo item type by the amount the item holds
			const EWeaponType AmmoType = AmmoItemObject->AmmoTypeByWeapon;
			UE_LOG(LogTemp, Warning, TEXT("Ammo Type = %s"), *UEnum::GetValueAsString(AmmoType));
			this->AmmoAmountsMap[AmmoItemObject->AmmoTypeByWeapon] += AmmoItemObject->CurrentTotalAmmo;
		}
	}
	// Called to refresh ammo items ammo amounts text widgets values
	this->OnInventoryChangedDel.Broadcast();
}

// Called to decrease ammo amount of a weapon type (ammo type) with a given amount to decrease
void UInventory::DecreaseAmmoByAmmoType(const EWeaponType InWeaponType, int32 InAmmoAmount)
{ // O(N) : N = 0...Items.Num()
	if (InAmmoAmount == 0) return;
	
	for (UItemObject* ItemObject : this->Items)
	{
		UAmmoItemObject* AmmoItemObject = Cast<UAmmoItemObject>(ItemObject);
		/*
		  Check if it is an ammo item, and that is holds ammo amount > 0
		  & that it's ammo type is the same as the given one
		 */
		if (AmmoItemObject != nullptr && AmmoItemObject->AmmoTypeByWeapon == InWeaponType
			&& AmmoItemObject->CurrentTotalAmmo > 0)
		{
			// The amount to take from ammo item is min between given amount and amount in the item
			const int32 AmountToTake = FMath::Min(InAmmoAmount, AmmoItemObject->CurrentTotalAmmo);
			// Decreasing from the given amount by the amount to be taken
			InAmmoAmount -= AmountToTake;
			// Decreasing from the actual ammo item by the amount to be taken
			AmmoItemObject->CurrentTotalAmmo -= AmountToTake;
			AmmoItemObject->AmmoAmountToAdd = AmmoItemObject->CurrentTotalAmmo;
			// Check if amount left in the ammo item is 0 or less
			if (AmmoItemObject->CurrentTotalAmmo <= 0)
			{
				// Removing the ammo item
				this->RemoveItem(ItemObject, false, false);
			}
		}
		// If the given amount reached 0 or less
		if (InAmmoAmount <= 0)
		{
			// Stop the loop
			break;
		}
	}
	// Update ammo amounts in the inventory
	this->UpdateAmmoAmounts();
}

// Called to check if removing an item can give a room for new item
bool UInventory::TestRemoveItemCheckRoomForNewItem(UItemObject* InItemObjectToRemove, UItemObject* InItemObjectToAdd)
{ // O(3 * N) : N = 0...Items.Num()
	// Both given items has to be valid
	if (InItemObjectToRemove == nullptr || InItemObjectToAdd == nullptr) return false;

	// Copy of the items array
	TArray<UItemObject*> ItemsCopy = this->Items;
	// Whether the item object has been found or not
	bool bFound = false;
	// Iterating for the copied array
	for (int32 i = 0; i < ItemsCopy.Num(); i++)
	{
		// Check if the same item then remove it from the copied array
		if (ItemsCopy[i] != nullptr && ItemsCopy[i]->GetItemId() == InItemObjectToRemove->GetItemId())
		{
			ItemsCopy[i] = nullptr;
			// The item to remove has been found
			if (!bFound) bFound = true;
		}
	}

	// If the item to remove has not been found then return false as 
	if (!bFound) return false;

	// Iterating to see if possible to add the new item to the copied array 
	for (int32 TopLeftIndex = 0; TopLeftIndex < ItemsCopy.Num(); TopLeftIndex++)
	{
		// Check if room is available for the given item to add
		if (IsRoomAvailable(InItemObjectToAdd, TopLeftIndex))
		{
			// there's a room for the given item to be added after removing the given item to remove
			return true;
		}
	}
	
	const FIntPoint ItemDimensions = InItemObjectToAdd->GetDimensions();
	// If x = y, no need to rotate, return false
	if (ItemDimensions.X == ItemDimensions.Y <= 1) return false;
	
	// Now check the same thing but with the given item to add is being rotated
	InItemObjectToAdd->RotateIcon();
	
	for (int32 TopLeftIndex = 0; TopLeftIndex < ItemsCopy.Num(); TopLeftIndex++)
	{
		if (IsRoomAvailable(InItemObjectToAdd, TopLeftIndex))
		{
			return true;
		}
	}
	return false;
}


/*
  Gamepad navigation methods
  The engine doesn't fully support navigation (basically means
  navigating between different widgets-UI elements) inside a
  dynamic sized elements grid widget.
  so these methods determine the act of any right-left-up-down navigating behaviour of the
  inventory grid widget (the items container).
 */

/*
  Called to find the next item of a given item, when the user hits left arrow/ stick
  using a gamepad device
 */
UItemObject* UInventory::NextItemRightNavigation(UItemObject* InItemObject)
{ // O(N ^ 2 + N) : N = 0...Items.Num()
	if (InItemObject == nullptr) return InItemObject;

	// Finding the item object top right index in the items array
	int32 RightTopIndex = 0; const int32 YDimension = InItemObject->GetDimensions().Y;
	for (int32 i = 0; i < this->Items.Num(); i++)
	{
		if (this->Items[i] == InItemObject)
		{
			RightTopIndex = i + InItemObject->GetDimensions().X - 1;
			break;
		}
	}

	int32 Rounds = 0;
	for (int32 i = RightTopIndex + 1; i < this->Items.Num(); i++)
	{
		if ((i+1) % this->Columns == 0 && Rounds == 0)
		{
			++Rounds;
			// To ignore same item object
			i += this->Columns * YDimension;
			// Check if i reached invalid index value
			if (i >= this->Items.Num())
			{
				// Iterating from the beginning
				for (int32 j = 0; j < this->Items.Num(); j++)
				{
					// Check if item object is valid
					if (this->Items[j] != nullptr)
					{
						// Return the item object 'widget' to be focused on 
						return this->Items[j];
					}
				}
			}
		}
		// Check if item object is valid & is not the same as the previous focused on item object 'widget'
		if (this->Items[i] != nullptr && this->Items[i] != InItemObject)
		{
			return this->Items[i];
		}
	}
	// No other item found, return the same item object
	return InItemObject;
}

/*
  Called to find the next item of a given item, when the user hits up arrow/ stick
  using a gamepad device
 */
UItemObject* UInventory::NextItemLeftNavigation(UItemObject* InItemObject)
{ // O(N ^ 2 + N) : N = 0...Items.Num()
	if (InItemObject == nullptr) return InItemObject;

	// Finding the item object bottom left index in the items array 
	int32 BottomLeftIndex = 0; const int32 YDimension = InItemObject->GetDimensions().Y;
	for (int32 i = 0; i < this->Items.Num(); i++)
	{
		if (this->Items[i] == InItemObject)
		{
			BottomLeftIndex = i + this->Columns * (YDimension - 1);
			break;
		}
	}

	int32 Rounds = 0;
	for (int32 i = BottomLeftIndex - 1; i >= 0; i--)
	{
		if ((i - 1) % this->Columns == 0 && Rounds == 0)
		{
			++Rounds;
			// To ignore same item object
			i -= this->Columns * (YDimension - 1);
			// Check if i reached invalid index value
			if (i < 0)
			{
				// Iterating from the end
				for (int32 j = this->Items.Num() - 1; j >= 0; j--)
				{
					// Check if item object is valid
					if (this->Items[j] != nullptr)
					{
						// Return the item object 'widget' to be focused on 
						return this->Items[j];
					}
				}
			}
		}
		// Check if item object is valid & is not the same as the previous focused on item object 'widget'
		if (this->Items[i] != nullptr && this->Items[i] != InItemObject)
		{
			return this->Items[i];
		}
	}
	// No other item found, return the same item object
	return InItemObject;
}

/*
  Called to find the next item of a given item, when the user hits up arrow/ stick
  using a gamepad device
 */
UItemObject* UInventory::NextItemUpNavigation(UItemObject* InItemObject)
{ // O(3 * N) : N = 0...Items.Num()
	if (InItemObject == nullptr) return InItemObject;

	// Finding the item object bottom right & top left indexes in the items array 
	int32 TopLeftIndex = 0, BottomRightIndex = 0;
	const FIntPoint Dimensions = InItemObject->GetDimensions();
	for (int32 i = 0; i < this->Items.Num(); i++)
	{
		if (this->Items[i] == InItemObject)
		{
			TopLeftIndex = i;
			BottomRightIndex = i + Dimensions.X - 1 + (Dimensions.Y - 1) * this->Columns;
			break;
		}
	}

	// Whether we passed a raw or not
	bool bNewRow = false;
	for (int32 i = TopLeftIndex - 1; i > 0; i--)
	{
		if (!bNewRow)
		{
			bNewRow = (i + 1) % this->Columns == 0;
		}
		/*
		  Check we passed 1 row means we going up & check if item object is valid
		  & is not the same as the previous focused on item object 'widget' 
		 */
		if (bNewRow && this->Items[i] != nullptr && this->Items[i] != InItemObject)
		{
			// Return the item object 'widget' to be focused on 
			return this->Items[i];
		}
	}

	// Iterating from the end
	for (int32 i = this->Items.Num() - 1; i > BottomRightIndex; i--)
	{
		// Check if item object is valid
		if (this->Items[i] != nullptr)
		{
			// Return the item object 'widget' to be focused on 
			return this->Items[i];
		}
	}
	// No other item found, return the same item object
	return InItemObject;
}

/*
  Called to find the next item of a given item, when the user hits down arrow/ stick
  using a gamepad device
 */
UItemObject* UInventory::NextItemDownNavigation(UItemObject* InItemObject)
{ // O(3 * N) : N = 0...Items.Num()
	if (InItemObject == nullptr) return InItemObject;

	// Finding the item object bottom right & top left indexes in the items array 
	int32 BottomRightIndex = 0, TopLeftIndex = 0;
	const FIntPoint Dimensions = InItemObject->GetDimensions();
	for (int32 i = 0; i < this->Items.Num(); i++)
	{
		if (this->Items[i] == InItemObject)
		{
			TopLeftIndex = i;
			BottomRightIndex = i + Dimensions.X - 1 + (Dimensions.Y - 1) * this->Columns;
			break;
		}
	}

	// Whether we passed a raw or not
	bool bNewRow = false;
	for (int32 i = BottomRightIndex + 1; i < this->Items.Num(); i++)
	{
		if (!bNewRow)
		{
			bNewRow = i % this->Columns == 0;
		}
		/*
		  Check we passed 1 row means we going up & check if item object is valid
		  & is not the same as the previous focused on item object 'widget' 
		 */
		if (bNewRow && this->Items[i] != nullptr && this->Items[i] != InItemObject)
		{
			// Return the item object 'widget' to be focused on 
			return this->Items[i];
		}
	}

	// Iterating from the beginning
	for (int i = 0; i < TopLeftIndex; i++)
	{
		// Check if item object is valid
		if (this->Items[i] != nullptr)
		{
			return this->Items[i];
		}
	}
	// No other item found, return the same item object
	return InItemObject;
}

// Called to initialize the inventory
void UInventory::InitializeInventory()
{ // O(1) without loading,
	// with loading from save slot: O(N) : N = 0...SavedItems.Num()
	
	// Limiting the inventory to 20 * 20 = 400 indexes
	this->Columns = FMath::Min(this->Columns, 20);
	this->Rows = FMath::Min(this->Rows, 20);

	// Creating new items array
	this->Items.SetNum(this->Columns * this->Rows);

	/*
	  Initialize the ammo amounts map
	  (for each ammo type => ammo amount), starts with 0
	 */
	this->AmmoAmountsMap.Add(Weapon_Pistol_Smg, 0);
	this->AmmoAmountsMap.Add(Weapon_AssaultRifle, 0);
	this->AmmoAmountsMap.Add(Weapon_Shotgun, 0);
	this->AmmoAmountsMap.Add(Weapon_Sniper, 0);
	this->AmmoAmountsMap.Add(Weapon_GrenadeLauncher, 0);

	// Called to load inventory items
	// this->LoadInventory();
}

void UInventory::LoadInventory()
{
	// Save game has been cancelled so the game will never save gameplay walk-through
	// means for now this method stops here
	if (!UGameplayStatics::DoesSaveGameExist("MySlot", 0)) return;
	
	const UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot("MySlot", 0));
	TArray<UItemObject*> SavedItems = SaveGameInstance->InvItems;
	TMap<FName, int32> AmmoItemObjectsAmmoAmounts = SaveGameInstance->InvAmmoItemObjectsAmmoAmount;
	TMap<FName, int32> GunsItemObjectsCurrentAmmoAmount = SaveGameInstance->InvGunsItemObjectsCurrentAmmoAmount;
	TMap<FName, int32> RightGunsItemObjectsCurrentAmmoAmount = SaveGameInstance->InvRightGunsItemObjectsCurrentAmmoAmount;
	for (int32 i = 0; i < SavedItems.Num(); i++)
	{
		if (SavedItems[i] != nullptr)
		{
			this->Items[i] = SavedItems[i];
			const FName CurrentItemObjectId = this->Items[i]->GetItemId();
			if (UAmmoItemObject* AmmoItemObject = Cast<UAmmoItemObject>(this->Items[i]))
			{
				if (AmmoItemObjectsAmmoAmounts.Contains(CurrentItemObjectId))
				{
					const int32 Amount = AmmoItemObjectsAmmoAmounts[CurrentItemObjectId];
					AmmoItemObject->CurrentTotalAmmo = Amount;
				}
			}
			if (UWeaponItemObject* WeaponItemObject = Cast<UWeaponItemObject>(this->Items[i]))
			{
				if (GunsItemObjectsCurrentAmmoAmount.Contains(CurrentItemObjectId))
				{
					WeaponItemObject->GunObjectCurrentAmmo = GunsItemObjectsCurrentAmmoAmount[CurrentItemObjectId];
				}
				if (RightGunsItemObjectsCurrentAmmoAmount.Contains(CurrentItemObjectId))
				{
					WeaponItemObject->RightHandGunCurrentAmmo = RightGunsItemObjectsCurrentAmmoAmount[CurrentItemObjectId];
				}
			}
		}
	}
	this->UpdateAmmoAmounts();
}
