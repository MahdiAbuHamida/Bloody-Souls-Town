// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/InteractableActors/Items/Enumurations/ItemType.h"
#include "ItemObject.generated.h"

class AItem;

UCLASS(Blueprintable)
class BLOODYSOULSTOWN_API UItemObject : public UObject
{
	GENERATED_BODY()	
	
public:
	// Initialize the item object properties
	void Init(const FName InItemId, FIntPoint InDimensions, UMaterialInterface* InIcon, TSubclassOf<AItem> InItemClass,
		UMaterialInterface* InIconRotated, FString InItemName, FString InItemInfo);

	
	/* Getters */

	// Method returns the item object id
	UFUNCTION(BlueprintPure)
	FName GetItemId() const;

	// Method returns the dimensions of the item object (icon dimensions)
	UFUNCTION(BlueprintPure)
	FIntPoint GetDimensions() const;

	// Method returns the icon of the item object
	UFUNCTION(BlueprintPure)
	UMaterialInterface* GetIcon() const;

	// Method returns the item class
	UFUNCTION(BlueprintPure)
	TSubclassOf<AItem> GetItemClass() const;

	// Method returns whether the item object icon is rotated or not
	bool GetIsIconRotated() const;

	// Method returns the item object name
	UFUNCTION(BlueprintPure)
	FString GetItemName() const;

	// Method returns info about the item
	UFUNCTION(BlueprintPure)
	FString GetItemInfo() const;

	// Method returns the item type
	UFUNCTION(BlueprintPure)
	EItemType GetItemType() const;
	

	/* Setters */

	// Method to set the item object id
	void SetItemId(const FName InItemId);

	// Method to set the item index
	void SetItemIndex(const int32 InIndex);

	/*
      Method that rotates the Item object
      used to check if there's a space in the player's inventory,
      after regular iterating failed, by rotating the item object,
      rotating is just replacing between x (width) and y (height) values.
     */
	UFUNCTION(BlueprintCallable)
	void RotateIcon();
	
protected:
	FName ItemId;
	
	EItemType ItemType;
	
	FIntPoint Dimensions;

	// Default icon
	UPROPERTY()
	UMaterialInterface* Icon;

	// Rotated icon
	UPROPERTY()
	UMaterialInterface* IconRotated;

	// The actual class of the item (actor, not the item object)
	TSubclassOf<AItem> ItemClass;
	
	bool bIsIconRotated = false;
	
	FString ItemName;
	
	FString ItemInfo;

	int32 ItemIndex;
};