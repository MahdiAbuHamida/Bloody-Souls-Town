// Mahdi Abu Hamida - Final University Project (2022)


#include "RegularDoor.h"

#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"


ARegularDoor::ARegularDoor()
{
	// Creating box collision component and attaching it to the root component
	this->DoorBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	this->DoorBox->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);
	this->DoorBox->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
	this->DoorBox->OnComponentBeginOverlap.AddDynamic(this, &ARegularDoor::OnDoorBoxBeginOverlap);
	this->DoorBox->OnComponentEndOverlap.AddDynamic(this, &ARegularDoor::OnDoorBoxEndOverlap);
	this->DoorBox->SetupAttachment(this->RootComponent);

	// Creating the audio component
	this->DoorAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	this->DoorAudioComponent->SetupAttachment(this->RootComponent);

	/*
	  Door sounds names (each name is associated with a sound source)
	  ***sounds are asserted in the blueprint editor of this class
	 */
	this->DoorSoundsCollection.Add(this->DoorOpeningSfxName);
	this->DoorSoundsCollection.Add(this->DoorClosingSfxName);

	// Creating arrow forward direction component for when to spawning an enemy spawner (where to spawn)
	this->EnemySpawnerArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Enemy Spawner Arrow Comp"));
	this->EnemySpawnerArrowComponent->SetupAttachment(this->RootComponent);

	// Needs save rotations and doors state later
	this->IsOpen = false;
	this->IsReadyState = true;

	// Interactable actor type is set to a door
	this->InteractableType = EInteractableActorType::Interactable_Door;
}

// Called every frame
void ARegularDoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	this->DoorTimeLine.TickTimeline(DeltaSeconds);
}

// Called when the game starts or when spawned
void ARegularDoor::BeginPlay()
{
	Super::BeginPlay();

	// Assigning the initial rotation yaw value of the door's mesh component rotation yaw value
	this->InitialRotationYaw = this->StaticMeshComponent->GetRelativeRotation().Yaw;

	// Check if this is a double doors
	if (this->bDoubleDoors)
	{
		// Getting all the doors in the world
		TArray<AActor*> Doors;
		UGameplayStatics::GetAllActorsOfClass(this, this->GetClass(), Doors);

		// Finding the 2nd door of this
		for (AActor* Actor : Doors)
		{
			if (Actor != this)
			{
				if (ARegularDoor* Door = Cast<ARegularDoor>(Actor))
				{
					if (Door->GetSecondDoorId() == this->SecondDoorId)
					{
						this->SecondDoor = Door;
						break;
					}
				}
			}
		}
	}

	// No need for a sphere shape collision for doors (added from the interactable parent class)
	this->CollisionSphereComponent->DestroyComponent();
	// Rotate value of the door (could be negative - inversed)
	this->RotateValue = 1.0f;
	
	if (this->DoorCurve != nullptr)
	{
		FOnTimelineFloat TimelineFloatCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;

		// binding a function to be called which receives the value of timeline curve
		TimelineFloatCallback.BindUFunction(this, FName("ControlDoor"));
		// binding a function to be called on timeline finished
		TimelineFinishedCallback.BindUFunction(this, FName("OnTimelineFinished"));
		/*
		  Adding the linear curve float (edited in UE editor) to the timeline
		  (connected the timeline & the curve float & the function binding to be called on value change)
		 */
		this->DoorTimeLine.AddInterpFloat(this->DoorCurve, TimelineFloatCallback);
		// Setting the timeline finished function binding to the timeline
		this->DoorTimeLine.SetTimelineFinishedFunc(TimelineFinishedCallback);
	}
}

/*
  Called to open the door in a direction based on
  the player location and the door's location
 */
void ARegularDoor::ToggleDoor(const AActor* OverlappedActor)
{
	if (OverlappedActor == nullptr || !this->IsReadyState)
	{
		return;
	}
	
	this->IsOpen = !this->IsOpen;
	// Determine if the player is in front or behind the door
	FVector Direction = this->GetActorLocation() - OverlappedActor->GetActorLocation();
	// Calculate direction based on root component rotation
	Direction = UKismetMathLibrary::LessLess_VectorRotator(Direction, this->GetActorRotation());
	this->DoorRotation = this->StaticMeshComponent->GetRelativeRotation();

	// Check if door is open
	if (this->IsOpen)
	{
		// Check if direction forward vector is positive (player is behind the door)
		if (Direction.X > 0.f)
		{
			// Check if inverse (door opens in inversed direction of the player) 
			this->Inverse ? this->RotateValue = -1.f : this->RotateValue = 1.f;
		}
		else
		{
			this->Inverse ? this->RotateValue = 1.f : this->RotateValue = -1.f;
		}
		// The door is opening so it is not ready for another toggle
		this->SetDoorState(false);
		// Start the timeline to rotate the door
		this->DoorTimeLine.PlayFromStart();
		this->TryPlayOpenDoorSfx();
	}
	// Otherwise, reverse the timeline 
	else
	{
		this->SetDoorState(false);
		this->DoorTimeLine.Reverse();
	}
}

// Method returns whether the door is open or not
bool ARegularDoor::IsDoorOpen() const
{
	return this->IsOpen;
}

// Method returns whether to hide the door's interaction widget after being opened or not
bool ARegularDoor::GetHideWidgetAfterOpened() const
{
	return this->bHideWidgetAfterOpened;
}

// Method returns whether to show the door's interaction widget or not
bool ARegularDoor::IsShowWidget() const
{
	return this->bShowWidget;
}

// Method returns the second door (like in double doors)
ARegularDoor* ARegularDoor::GetSecondDoor() const
{
	return this->SecondDoor;
}

// Method returns the second door id
FName ARegularDoor::GetSecondDoorId() const
{
	return this->SecondDoorId;
}

// Method returns whether to spawn an enemy spawner after opening the door or not
bool ARegularDoor::IsEnemySpawnerAfterOpenedAllowed() const
{
	return this->bAllowToSpawnEnemySpawner;
}

/*
  Method returns the arrow forward direction component of the enemy spawner
  enemy spawner which could be spawned after the door opens
 */
UArrowComponent* ARegularDoor::GetDoorEnemySpawnerArrowComponent() const
{
	return this->EnemySpawnerArrowComponent;
}

// Method returns the enemy spawner class
UClass* ARegularDoor::GetEnemySpawnerClass() const
{
	return this->EnemySpawnerClass;
}

// Method to set whether to show the interaction widget or not
void ARegularDoor::SetShowWidget(const bool InShowWidget)
{
	this->bShowWidget = InShowWidget;
}

/*
  Called every frame of the door's rotating-linear-curve timeline
  to update the door's rotation based on the timeline value
 */
void ARegularDoor::ControlDoor()
{
	this->TimelineValue = this->DoorTimeLine.GetPlaybackPosition();
	this->CurveFloatValue = this->RotateValue * this->DoorCurve->GetFloatValue(this->TimelineValue);

	const FQuat NewRotation = FQuat(FRotator(0.f, this->CurveFloatValue, 0.f));
	this->StaticMeshComponent->SetRelativeRotation(NewRotation);
}

// Setting the door state
void ARegularDoor::SetDoorState(const bool bReadyState)
{
	this->IsReadyState = bReadyState;
}

// Called when the timeline finishes to set the door state
void ARegularDoor::OnTimelineFinished(const bool bReadyState)
{
	this->SetDoorState(bReadyState);

	/*
	  Check whether the door is rotated with an absolute value bigger than 40
	  to update if AI can get in or not
	 */
	const bool bEnable = this->StaticMeshComponent->CanEverAffectNavigation() && FMath::Abs(
		this->StaticMeshComponent->GetRelativeRotation().Yaw - this->InitialRotationYaw) > 40.f;
	
	this->StaticMeshComponent->SetCanEverAffectNavigation(bEnable);
	
}

// Called to try to play a sfx
void ARegularDoor::TryPlayOpenDoorSfx()
{
	// Stop the current audio if is playing
	if (this->DoorAudioComponent->IsPlaying()) this->DoorAudioComponent->Stop();

	// Check if there's an open door sfx to play
	if (this->DoorSoundsCollection.Contains(this->DoorOpeningSfxName)
		&& this->DoorSoundsCollection[this->DoorOpeningSfxName] != nullptr)
	{
		// Setting a new sound to play
		this->DoorAudioComponent->SetSound(this->DoorSoundsCollection[this->DoorOpeningSfxName]);
		this->DoorAudioComponent->Play();
	}
}

// Called when another actor overlaps with the door's box component's collision
void ARegularDoor::OnDoorBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	/*
	 * Check if overlap event is valid and the door can be opened
	 * and does automatically opens, then continue
	 */
	if (OtherComponent != nullptr && OtherActor != this && OtherActor != nullptr && this->bCanDoorBeOpened && this->bDoorAutomaticallyOpens)
	{
		APawn* OverlappedPawn = Cast<APawn>(OtherActor);
		if (Cast<AActor>(OverlappedPawn) != OtherActor) return;

		// Check if the other actor is the player
		if (OverlappedPawn == UGameplayStatics::GetPlayerPawn(this, 0))
		{
			// Toggling the door
			if (!this->IsOpen && this->bDoorAutomaticallyOpens)
			{
				this->ToggleDoor(OverlappedPawn);
			}
			else if (this->IsOpen && this->bCanDoorBeClosedAfterOpened)
			{
				this->ToggleDoor(OverlappedPawn);
			}
		}
		/*
		 * otherwise means that other actor is a pawn which means it's an enemy character.
		 * check if the door can be opened by enemies
		 */
		else if (this->bCanDoorAutoOpenByAI)
		{
			if (!this->IsOpen && this->bDoorAutomaticallyOpens)
			{
				this->ToggleDoor(OverlappedPawn);
			}
			else if (this->IsOpen && bCanDoorBeClosedAfterOpened)
			{
				this->ToggleDoor(OverlappedPawn);
			}
		}
	}
}

// Called when another actor leaves the door's box component's collision area
void ARegularDoor::OnDoorBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != this && OtherActor != nullptr && OtherComp != nullptr && this->bDoorAutomaticallyCloses)
	{
		APawn* OverlappedPawn = Cast<APawn>(OtherActor);
		if (Cast<AActor>(OverlappedPawn) != OtherActor) return;
		
		if (OverlappedPawn == UGameplayStatics::GetPlayerPawn(this, 0))
		{
			if (this->bDoorAutomaticallyCloses)
			{
				ToggleDoor(OverlappedPawn);
			}
		}
		else if (this->bCanDoorAutoCloseByAI)
		{
			ToggleDoor(OverlappedPawn);
		}
	}
}
