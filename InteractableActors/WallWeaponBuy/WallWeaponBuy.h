// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/InteractableActors/Interactable.h"
#include "Components/BoxComponent.h"
#include "BloodySoulsTown/InteractableActors/Items/Weapons/Weapon.h"
#include "WallWeaponBuy.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API AWallWeaponBuy final : public AInteractable
{
	GENERATED_BODY()

public:
	AWallWeaponBuy()
	{
		this->InteractableType = Interactable_WallWeapon;
		
		// Setting collision response to ignore projectiles (like bullets)
		this->StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECR_Ignore);
		this->StaticMeshComponent->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
		// Creating box collision component and attaching it to the root component
		this->BoxCollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
		this->BoxCollisionComp->SetCollisionObjectType(ECC_GameTraceChannel3);
		this->BoxCollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
		this->BoxCollisionComp->SetRelativeLocation(FVector(9.0f, -8.0f, 0.0f));
		this->BoxCollisionComp->SetBoxExtent(FVector(5.0f, 50.0f, 20.0f));
		this->BoxCollisionComp->SetupAttachment(this->RootComponent);
	}

	
	/* Getters */

	// Method returns the weapon class
	TSubclassOf<AWeapon> GetWeaponClass() const
	{
		return this->WeaponClass;
	}

	// Method returns the max cost of buying an ammo in a wall weapon buy (while having same weapon)
	int32 GetAmmoIncreaseForSameWeaponCost() const
	{
		return this->AmmoIncreaseForSameWeaponCost;
	}

	// Copy of the original action text of the interaction widget
	FText ActionTextCopy;

	// Copy of the original cost
	int32 CostCopy;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override
	{
		Super::BeginPlay();
		
		// This class uses box as collision not sphere from the inherited class
		this->CollisionSphereComponent->DestroyComponent();
		
		this->ActionTextCopy = this->WidgetActionText;
		this->CostCopy = this->Cost;
	}

	// Box collision component
	UPROPERTY(EditDefaultsOnly, Category = "ActorComponents")
	UBoxComponent* BoxCollisionComp;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeapon> WeaponClass;

	// Max cost of buying an ammo in a wall weapon buy (while having same weapon)
	UPROPERTY(EditDefaultsOnly, Category = "Player Interaction")
	int32 AmmoIncreaseForSameWeaponCost;
};
