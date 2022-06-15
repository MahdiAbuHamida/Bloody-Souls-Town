// Mahdi Abu Hamida - Final University Project (2022)


#include "BloodySoulsTown/PlayerComponents/QuestHandler/ShieldActorComponent.h"

#include "BloodySoulsTown/Characters/AlexCharacter.h"
#include "BloodySoulsTown/PlayerComponents/Inventory/QuestInventory/QuestInventory.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AShieldActorComponent::AShieldActorComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CurrentProtectionValue = MaxProtectionValue;

	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shield Mesh"));
	SetRootComponent(ShieldMesh);
}

// Called when the game starts or when spawned
void AShieldActorComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool AShieldActorComponent::IsShieldDestroyed() const
{
	return CurrentProtectionValue == 0.f;
}

float AShieldActorComponent::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HitShieldSfx != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitShieldSfx, GetActorLocation(),
			GetActorRotation(), 1, 1, 0,
			HitShieldSfx->AttenuationSettings, nullptr, this);
	}
	
	const float DamageToApply = CurrentProtectionValue - FMath::Max(CurrentProtectionValue - DamageAmount, 0.f);
	CurrentProtectionValue -= DamageToApply;

	if (IsShieldDestroyed())
	{
		if (AAlexCharacter* PlayerCharacter = Cast<AAlexCharacter>(UGameplayStatics::GetPlayerPawn(
			this, 0)))
		{
			PlayerCharacter->SetHasShield(false);
			if (UQuestInventory* QuestInventory = PlayerCharacter->GetPlayerQuestInventory())
			{
				QuestInventory->RemoveItem(QuestItemObject);
			}
		}
		Destroy();
	}

	return DamageToApply;
}

void AShieldActorComponent::SetItemObject(UItemObject* InItemObject)
{
	QuestItemObject = InItemObject;
}
