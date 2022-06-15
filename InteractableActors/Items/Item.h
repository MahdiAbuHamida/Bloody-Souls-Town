// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/InteractableActors/Interactable.h"
#include "Enumurations/ItemType.h"
#include "Item.generated.h"

class UItemObject;

UCLASS()
class BLOODYSOULSTOWN_API AItem : public AInteractable
{
	GENERATED_BODY()

public:
	AItem();

	
	/* Getters */

	// Method returns the item object
	UItemObject* GetItemObject() const;

	// Method returns the item id
	FName GetItemId() const;

	// Method returns whether the item is currently spawning or not
	bool GetIsInSpawningState() const;

	
	/* Setters */
	
	// Method to set the item id
	void SetItemId(const FName InItemId) const;

	// Method to set the item object
	void SetItemObject(UItemObject* InItemObject);

	// Method to set whether the item is currently spawning or not
	void SetIsInSpawningState(const bool InIsInSpawningState);

	
	EItemType ItemType;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	class USoundCue* PickupSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to initialize the item object
	UFUNCTION(BlueprintPure)
	virtual UItemObject* GetDefaultItemObject();

	
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn="true"))
	UItemObject* ItemObject;
	
	FName ItemId;
	
	UPROPERTY(EditDefaultsOnly, Category = "Item Object Properties")
	UMaterialInterface* Icon;
	
	UPROPERTY(EditAnywhere, Category = "Item Object Properties")
	UMaterialInterface* IconRotated;

	UPROPERTY(EditDefaultsOnly, Category = "Item Object Properties")
	TSubclassOf<AItem> ItemClass;

	UPROPERTY(EditAnywhere, Category = "Item Object Properties")
	FIntPoint Dimensions;

	UPROPERTY(EditAnywhere, Category = "Item Object Properties")
	FString ItemName;

	UPROPERTY(EditAnywhere, Category = "Item Object Properties")
	FString ItemInfo;
	
	// Whether to use outline effect or not
	UPROPERTY(EditDefaultsOnly, Category = "OutlineEffect")
	bool bUseRenderCustomDepth = true;

private:
	// Called to check if the item has been already destroyed
	UFUNCTION()
	void CheckIfAlreadyDestroyed();
	
	FTimerHandle TimerHandle_CheckIfDestroyed;

	// Whether the item is in a spawn state or not
	bool IsInSpawningState = false;
};