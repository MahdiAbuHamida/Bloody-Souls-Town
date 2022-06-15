// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "PowerStoneType.h"
#include "BloodySoulsTown/InteractableActors/Interactable.h"
#include "PowerStone.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API APowerStone final : public AInteractable
{
	GENERATED_BODY()

public:
	APowerStone();

	/* Getters */

	// Method returns the type of the power stone
	EPowerStoneType GetPowerStoneType() const;

	// Method returns the power the power stone gives
	float GetPowerValue() const;

	// Method returns the texture icon of the power stone
	UFUNCTION(BlueprintPure)
	UTexture2D* GetPowerStoneIcon() const;

protected:
	// Plane static mesh component which represents 3D text above the stone
	UPROPERTY(EditDefaultsOnly, Category = "Default Properties")
	UStaticMeshComponent* PlaneMesh;

	// The type of the power stone
	UPROPERTY(EditDefaultsOnly, Category = "Default Properties")
	TEnumAsByte<EPowerStoneType> PowerStoneType;

	// The power value this power stone gives 
	UPROPERTY(EditDefaultsOnly, Category = "Default Properties")
	float PowerValue;

	// The texture icon of the power stone
	UPROPERTY(EditDefaultsOnly, Category = "UserInterface")
	UTexture2D* PowerStoneIcon;
};
