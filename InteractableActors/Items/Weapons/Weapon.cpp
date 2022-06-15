// Mahdi Abu Hamida - Final University Project (2022)


#include "Weapon.h"

#include "Components/BoxComponent.h"
#include "BloodySoulsTown/Characters/AlexCharacter.h"
#include "BloodySoulsTown/InteractableActors/Items/ItemObjects/WeaponItemObject.h"

AWeapon::AWeapon()
{
	this->ItemType = Item_Weapon;
	this->SavedActionText = this->WidgetActionText;

	this->BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCompInsideMysteryBox"));
	this->BoxCollision->InitBoxExtent(FVector(12.0f, 115.0f, 35.0f));
	this->BoxCollision->SetRelativeLocation(FVector(40.0f, 0.0f, 0.0f));
	this->BoxCollision->SetCollisionObjectType(ECC_GameTraceChannel3);
	this->BoxCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
	this->BoxCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	this->BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->BoxCollision->SetupAttachment(this->RootComponent);
}

// Method returns the box collision component for mystery box weapon
UBoxComponent* AWeapon::GetBoxComponent() const
{
	return this->BoxCollision;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called to check if weapon is in ready state to attack
bool AWeapon::PrepareAttack()
{
	return true;
}

// Called to attack
void AWeapon::Attack()
{
	//.. This parent class has no specific attack method.
}

/*
 * Stop attack method called when the player stops attacking.
 * The weapon will stop attack functionality once the last animation play is done. 
 */
void AWeapon::StopAttack()
{
	AAlexCharacter* PlayerCharacter = Cast<AAlexCharacter>(this->GetOwner());
	if (PlayerCharacter != nullptr && this->IsAutomaticWeapon)
	{
		GetWorldTimerManager().ClearTimer(PlayerCharacter->TimerHandle_ReFire);
		if (this->AttacksCounter == 0 && !PlayerCharacter->bIsNearWall)
		{
			this->Attack();
		}
	}
	this->AttacksCounter = 0;
}

void AWeapon::StopAttackLeft()
{
	AAlexCharacter* PlayerCharacter = Cast<AAlexCharacter>(this->GetOwner());
	if (PlayerCharacter != nullptr && this->IsAutomaticWeapon)
	{
		GetWorldTimerManager().ClearTimer(PlayerCharacter->TimerHandle_LeftReFire);
		if (this->AttacksCounter == 0)
		{
			this->Attack();
		}
	}
	this->AttacksCounter = 0;
}

UItemObject* AWeapon::GetDefaultItemObject()
{
	UWeaponItemObject* WeaponItemObject = NewObject<UWeaponItemObject>(this);
	WeaponItemObject->Init(this->ItemId, this->Dimensions, this->Icon, this->ItemClass,
		this->IconRotated, this->ItemName, this->ItemInfo, this->IsGun, this->WeaponInWheelImage,
		this->WeaponInWheelImageSize, this->RenderPivot, this->WeaponType, this->WeaponData);

	this->ItemObject = Cast<UItemObject>(WeaponItemObject);
	return this->ItemObject;
}


/* Getters */

// Method returns delay (seconds) between each attack
float AWeapon::GetDelayBetweenAttacks() const
{
	return this->DelayBetweenAttacks;
}

// Method returns player character skeletal mesh "weapon" socket name
FName AWeapon::GetPlayerWeaponSocketName() const
{
	return this->PlayerWeaponSocketName;
}

// Method returns player character skeletal mesh "left weapon" socket name
FName AWeapon::GetPlayerWeaponSocketNameLeftHand() const
{
	return this->PlayerWeaponSocketNameLeftHand;
}

// Method returns the weapon type
TEnumAsByte<EWeaponType> AWeapon::GetWeaponType() const
{
	return this->WeaponType;
}

// Method returns whether the weapon is explosive or not
bool AWeapon::GetIsExplosiveWeapon() const
{
	return this->IsExplosive;
}

// Method returns the impulse amount weapon applies on kill hit
float AWeapon::GetWeaponKillImpulse() const
{
	return this->KillImpulse;
}

// Method returns the weapon attack range
float AWeapon::GetWeaponAttackRange() const
{
	return this->Range;
}

// Method returns the weapon damage amount
float AWeapon::GetWeaponDamageAmount() const
{
	return this->DamageAmount;	
}


/* Setters */

// Method to set the delay between a rapid attacks (when player gets rapid fire power stone)
void AWeapon::SetDelayBetweenRapidAttacks()
{
	this->DelayBetweenAttacks = this->DelayBetweenRapidAttacks;
	this->WeaponData.FireRate = this->DelayBetweenAttacks;
}