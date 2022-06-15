// Mahdi Abu Hamida - Final University Project (2022)


#include "BloodySoulsTown/Characters/AI/TriggerBoxEnemySpawner.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BloodySoulsTown/Characters/AI/EnemySpawner.h"
#include "BloodySoulsTown/Other/MainGameMode.h"

// Sets default values
ATriggerBoxEnemySpawner::ATriggerBoxEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Creating the trigger box collision component
	this->TriggerBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box Comp"));
	this->SetRootComponent(this->TriggerBoxComp);

	// Creating the arrow component
	this->ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow Comp"));
	this->ArrowComp->SetupAttachment(this->RootComponent);

	/*
	  Adding begin overlap function to be called on when
	  other actor collides with the box component 
	 */
	this->TriggerBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ATriggerBoxEnemySpawner::OnBoxBeginOverlap);
}

// Called when the game starts or when spawned
void ATriggerBoxEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	// Validating the spawner class
	if (!IsValid(this->EnemySpawnerClass))
	{ // Write a console message that spawner class is a null pointer 
		UE_LOG(LogTemp, Warning, TEXT("TriggerBoxEnemySpawner.cpp line 33 ** Trigger Box Enemy Spawner Class Is nullptr !"));
		// Destroying this box trigger enemy AI spawner
		this->Destroy();
	}
}

// Called when other actor collides with the box component 
void ATriggerBoxEnemySpawner::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*
	  validating data of other actor which has to be the player actor
	  and both the enemy spawner class and arrow component cannot be null
	 */
	if (OtherActor != nullptr && OtherComp != nullptr && Cast<AActor>(
		UGameplayStatics::GetPlayerPawn(this, 0)) == OtherActor
		&& IsValid(this->EnemySpawnerClass) && this->ArrowComp != nullptr)
	{
		if (AMainGameMode* MainGameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			MainGameMode->SpawnEnemySpawner(this->ArrowComp->GetComponentLocation(), this->EnemySpawnerClass);
			this->Destroy();
		}
		// Otherwise write a console message that spawning an enemy spawner is failed
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("TriggerBoxEnemySpawner.cpp line 53 ** SpawnActor() returned a nullptr !"));
		}
	}
}

