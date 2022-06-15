// M&M Games 2021 - Free University Project

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "MyGameInstance.generated.h"

/**
 * 
 */

UCLASS()
class BLOODYSOULSTOWN_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMyGameInstance();
	
	void SaveGame();

	UPROPERTY(BlueprintReadWrite)
	bool IsGamepadKey = true;

	void DestroyItem(const FName ItemID);

	bool CheckIfAlreadyDestroyed(const FName ItemID) const;
	
	TArray<FName> DestroyedActors;

private:
	void UpdateItemsLocationsRotations(const UWorld* World);
	void UpdateGunsCurrentAmmoAmount(const UWorld* World);
	TMap<FName, int32> GetAmmoItemsAmmoAmount() const;

	TMap<FName, FVector> ItemsLocations;
	TMap<FName, FRotator> InteractableActorsRotations;
	TMap<FName, int32> GunsCurrentAmmoAmount;
	TMap<FName, int32> RightGunsCurrentAmmoAmount;
};