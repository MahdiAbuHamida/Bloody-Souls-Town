// Mahdi Abu Hamida - Final University Project (2022)


#include "PowerStone.h"

APowerStone::APowerStone()
{
	this->InteractableType = Interactable_PowerStone;

	// Creating the plane static mesh component
	this->PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlanceMesh"));
	this->PlaneMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->PlaneMesh->SetupAttachment(this->RootComponent);
}

// Method returns the type of the power stone
EPowerStoneType APowerStone::GetPowerStoneType() const
{
	return this->PowerStoneType;
}

// Method returns the power the power stone gives
float APowerStone::GetPowerValue() const
{
	return this->PowerValue;
}

// Method returns the texture icon of the power stone
UTexture2D* APowerStone::GetPowerStoneIcon() const
{
	return this->PowerStoneIcon;
}
