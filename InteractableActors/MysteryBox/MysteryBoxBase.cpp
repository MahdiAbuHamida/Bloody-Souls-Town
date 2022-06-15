// Mahdi Abu Hamida - Final University Project (2022)


#include "MysteryBoxBase.h"

#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "BloodySoulsTown/InteractableActors/Items/Weapons/Weapon.h"
#include "BloodySoulsTown/Other/MainGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/TransformCalculus3D.h"

AMysteryBoxBase::AMysteryBoxBase()
{
	this->InteractableType = Interactable_MysteryBox;
	
	// Current state of the Mystery Box is not open and not activated
	this->IsOpen = false;
	this->IsActive = false;
	
	// Initialize top part of the Mystery Box root component opening state target transforms
	this->BoxLocationY_Value = 85.0f;
	this->BoxLocationZ_Value = 169.0f;
	this->BoxRotationRoll_Value = -80.0f;
	
	// Initialize the point light component opening state target transforms & light's intensity
	this->BottomPointLightLocation = FVector(-3.5f, 24.0f, 30.0f);
	this->TopPointLightLocation = FVector(-3.5f, 24.0f, 200.0f);
	this->PointLightMaxIntensity = 1000.0f;
	
	/***
	 * Creating the root component of the top part of the box
	 * (the one that will be moving up/down & rotating)
	 ***/
	
	this->AnimatedSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("AnimatedScene"));
	// Attaching the root component of the top part of the box to the main root component 
	this->AnimatedSceneComp->SetupAttachment(this->RootComponent);
	
	/* Creating a point light component */
	
	this->PointLightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("POintLight"));
	// Default color of the point light component
	this->PointLightComp->SetLightColor(FLinearColor(1.0f, 0.630757f, 0.0f, 1.0f));
	// Default intensity of the point light component (0) 
	this->PointLightComp->SetIntensity(0.f);
	// Default attenuation radius of the point light component (100)
	this->PointLightComp->SetAttenuationRadius(100.0f);
	// Attaching the point light component to the main root component
	this->PointLightComp->SetupAttachment(this->RootComponent);

	
	// Creating the box collision component and attaching it to the main root component
	
	this->CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	// Default scale of the box component
	this->CollisionBox->InitBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	// Attaching the box component to the main root component
	this->CollisionBox->SetupAttachment(this->RootComponent);
}

// Called when the player buys from the Mystery Box
void AMysteryBoxBase::OpenBox()
{
	if (this->BoxWeaponsArray.Num() == 0) return;
	this->IsOpen = true;
	if (this->InteractionWidget->IsInViewport())
	{
		this->InteractionWidget->RemoveFromViewport();
	}
	
	FTimerManager& WorldTimerManager = this->GetWorldTimerManager();
	
	// Setting a timer for opening the box & raising the weapons
	WorldTimerManager.SetTimer(this->TimerHandle_OpenBox, this,
		&AMysteryBoxBase::OpenBoxRaiseWeapons, 0.02f, true);
	
	// Setting a timer for changing the selected weapon
	WorldTimerManager.SetTimer(this->TimerHandle_ChangeSelectedWeapon, this,
		&AMysteryBoxBase::ChangeSelectedWeapon, this->ChangeSelectedWeaponRate, true);

	// Setting a timer to stop changing the selected weapon
	this->GetWorldTimerManager().SetTimer(TimerHandle_StopChangingSelectedWeapon, this,
		&AMysteryBoxBase::StopChangeSelectedWeapon,StopChangeSelectedWeaponDelay, false);

	// Trying to play mystery box opening music
	if (this->MysteryBoxOpenMusic != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, this->MysteryBoxOpenMusic, this->GetActorLocation());
	}
}

// Called to activate the mystery box
void AMysteryBoxBase::ActivateMysteryBox()
{
	if (this->LightShaftParticleComp == nullptr)
	{
		this->AddLightShaft();
		this->IsActive = true;
	}
}

// Called to deactivate the mystery box
void AMysteryBoxBase::DeactivateMysteryBox()
{
	if (this->LightShaftParticleComp != nullptr)
	{
		this->LightShaftParticleComp->DestroyComponent();
		this->LightShaftParticleComp = nullptr;
		this->IsActive = false;
	}
}

// Called to notify the mystery box to deactivates while it is opened
void AMysteryBoxBase::NotifyToDeactivateMysteryBoxAfterClose()
{
	this->NotifiedAfterCloseToDeactivate = true;
}

// Called to close the mystery after the player got a weapon from it (immediate close)
void AMysteryBoxBase::ImmediateClose()
{
	FTimerManager& TimerManager = this->GetWorldTimerManager();
	TimerManager.ClearTimer(TimerHandle_OpenBox);
	TimerManager.ClearTimer(TimerHandle_LowerWeapons);
	TimerManager.ClearTimer(TimerHandle_ChangeSelectedWeapon);
	TimerManager.ClearTimer(TimerHandle_StopChangingSelectedWeapon);
	TimerManager.ClearTimer(TimerHandle_CloseBox);

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("LowerWeapons"), true);
	TimerManager.SetTimer(this->TimerHandle_LowerWeapons, TimerDelegate,
		0.02f, true, this->StartLowerWeaponsDelay);
}

void AMysteryBoxBase::BeginPlay()
{
	Super::BeginPlay();

	// The Mystery Box now uses a box collision, so destroy the inherited sphere collision component
	this->CollisionSphereComponent->DestroyComponent();
	// The Mystery Box uses more than 1 static mesh, the inherited mesh will be destroyed (no need for it) 
	this->StaticMeshComponent->DestroyComponent();

	this->CurrentTopBoxLocation = this->AnimatedSceneComp->GetRelativeLocation();
	this->CurrentTopBoxRotation = this->AnimatedSceneComp->GetRelativeRotation();

	/*** Getting default transforms for the weapons (that will be inside the Mystery Box) ***/
	const FVector BoxLocation = this->RootComponent->GetComponentLocation();
	this->BottomWeaponLocation = FVector(BoxLocation.X, BoxLocation.Y, BoxLocation.Z + 17.0f);
	this->CurrentWeaponLocation = this->BottomWeaponLocation;
	this->TopWeaponLocation = FVector(BoxLocation.X, BoxLocation.Y, BoxLocation.Z + 132.0f);

	const FRotator BoxRotation = this->GetActorRotation();
	const FRotator CurrentWeaponRotation(BoxRotation.Pitch, BoxRotation.Yaw + 90.0f, BoxRotation.Roll);

	// Settings up weapons to put them inside the box
	for (UClass* Class : this->WeaponsSelectionClasses)
	{
		if (AWeapon* Weapon = this->GetWorld()->SpawnActor<AWeapon>(Class))
		{
			/* Changing the weapon collision from a sphere to box collision */
			Weapon->GetSphereComponent()->DestroyComponent();
			
			// Setting the weapon location inside the mystery box
			Weapon->SetActorLocation(this->CurrentWeaponLocation);
			Weapon->SetActorRotation(CurrentWeaponRotation);
			Weapon->SetActorHiddenInGame(true);
			Weapon->MysteryBoxRef = this;
			
			// Adding the weapon to the box weapons array
			BoxWeaponsArray.Add(Weapon);
		}
	}
}

// Called to add the particle
void AMysteryBoxBase::AddLightShaft()
{
	if (this->LightShaftParticle != nullptr)
	{
		this->LightShaftParticleComp = NewObject<UParticleSystemComponent>(this);
		this->LightShaftParticleComp->Template = this->LightShaftParticle;
		this->LightShaftParticleComp->AttachToComponent(this->RootComponent,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		this->LightShaftParticleComp->SetRelativeTransform(this->LightShaftRelativeTransform);
		this->LightShaftParticleComp->RegisterComponent();
	}
}

/*
  Method called every x second to open the Mystery Box:
   * The top part of the Mystery Box (The root component of the meshes parts) which will be raised up
     (Z axis) and rotates by the roll axis.
   * The current selected weapon (along the time while the selected weapon will be changed) will be raised up
   * The Point light component's intensity will be increased, also it will be raised up (Z axis)
 */
void AMysteryBoxBase::OpenBoxRaiseWeapons()
{
	if (CurrentTopBoxLocation == FVector(0.0f, this->BoxLocationY_Value, this->BoxLocationZ_Value)
		&& CurrentTopBoxRotation == FRotator(0.0f, 0.0f, this->BoxRotationRoll_Value)
		&& CurrentLightLocation == TopPointLightLocation && CurrentPointLightIntensity == PointLightMaxIntensity
		&& CurrentWeaponLocation == TopWeaponLocation)
	{
		this->GetWorldTimerManager().ClearTimer(TimerHandle_OpenBox);
		return;
	}
	
	const float DeltaSeconds = this->GetWorld()->GetDeltaSeconds();
	// Calculate new different transforms & other properties for each part
	this->CurrentTopBoxLocation = UKismetMathLibrary::VInterpTo_Constant(this->CurrentTopBoxLocation,
		FVector(0.0f, this->BoxLocationY_Value, this->BoxLocationZ_Value), DeltaSeconds,
		OpenTopBoxLocationInterpolationSpeed);
	this->CurrentTopBoxRotation = UKismetMathLibrary::RInterpTo_Constant(this->CurrentTopBoxRotation,
		FRotator(0.0f, 0.0f, this->BoxRotationRoll_Value), DeltaSeconds,
		OpenTopBoxRotationInterpolationSpeed);
	this->CurrentLightLocation = UKismetMathLibrary::VInterpTo_Constant(this->CurrentLightLocation,
		this->TopPointLightLocation, DeltaSeconds, PointLightLocInterpolationSpeed);
	this->CurrentPointLightIntensity = UKismetMathLibrary::FInterpTo_Constant(this->CurrentPointLightIntensity,
		this->PointLightMaxIntensity, DeltaSeconds, PointLightIntensityInterpolationSpeed);
	this->CurrentWeaponLocation = UKismetMathLibrary::VInterpTo_Constant(this->CurrentWeaponLocation,
		this->TopWeaponLocation, DeltaSeconds, RaiseWeaponInterpolationSpeed);

	// Setting the new transforms & properties to each part
	this->AnimatedSceneComp->SetRelativeLocation(this->CurrentTopBoxLocation);
	this->AnimatedSceneComp->SetRelativeRotation(this->CurrentTopBoxRotation);
	this->PointLightComp->SetRelativeLocation(this->CurrentLightLocation);
	this->PointLightComp->SetIntensity(this->CurrentPointLightIntensity);

	for (AWeapon* Weapon : this->BoxWeaponsArray)
	{
		if (Weapon != nullptr)
		{
			Weapon->SetActorLocation(this->CurrentWeaponLocation);
		}
	}
}

/*
  Method called every x seconds which will select a new random weapon from the box,
  By assigning it & make it visible in-game (the previous weapon will set to be hidden in-game)
 */
void AMysteryBoxBase::ChangeSelectedWeapon()
{
	// If the weapons array length is less or equal to 0, stop the execution of the method and return
	if (this->BoxWeaponsArray.Num() <= 0) return;
	// Getting random index
	int8 RandomIndex = FMath::RandRange(0, this->BoxWeaponsArray.Num() - 1);
	// Check if the current selected weapon is not a null pointer
	if (this->SelectedWeapon != nullptr)
	{
		// Current selected weapon set to be hidden in-game
		this->SelectedWeapon->SetActorHiddenInGame(true);
		int32 Iterator = 0;
		// iterating as long as the weapon hasn't changed
		while (this->SelectedWeapon == this->BoxWeaponsArray[RandomIndex])
		{
			RandomIndex = FMath::RandRange(0, this->BoxWeaponsArray.Num() - 1);
			++Iterator;
			// Prevent performance drop
			if (Iterator >= 100) break;
		}
	}
	// Setting the new selected weapon from the random generated index in the weapons array
	this->SelectedWeapon = this->BoxWeaponsArray[RandomIndex];
	// Setting the new (current) selected weapon visibility in-game to not be hidden
	this->SelectedWeapon->SetActorHiddenInGame(false);
}

// Called to stop changing the selected weapon
void AMysteryBoxBase::StopChangeSelectedWeapon()
{
	FTimerManager& WorldTimerManager = this->GetWorldTimerManager();
	WorldTimerManager.ClearTimer(this->TimerHandle_ChangeSelectedWeapon);
	WorldTimerManager.ClearTimer(this->TimerHandle_StopChangingSelectedWeapon);
	
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("LowerWeapons"), false);
	WorldTimerManager.SetTimer(this->TimerHandle_LowerWeapons, TimerDelegate,
		0.02f, true, this->StartLowerWeaponsDelay);
	if (this->SelectedWeapon != nullptr)
	{
		if (UBoxComponent* WeaponBoxCollision = this->SelectedWeapon->GetBoxComponent())
		{
			/*
			  Resetting the collision response to the player's interaction line trace to block
			  means that whenever the player looks at the randomly selected weapon (it's box collision)
			  he will be able to interact with it (picks it up)
			 */
			WeaponBoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			WeaponBoxCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
		}
	}
}

// Called to lower the weapons
void AMysteryBoxBase::LowerWeapons(const bool InbImmediate)
{
	/* Closing mystery box different parts speeds based on if it is an immediate close or not */
	
	// L_W_I_S => Lower Weapon Interpolation Speed
	const float L_W_I_S = InbImmediate ? LowerWeaponsInterpolationSpeed * 5 : LowerWeaponsInterpolationSpeed;
	// P_L_L_I_S => Point Light Location Interpolation Speed
	const float P_L_L_I_S = InbImmediate ? PointLightLocInterpolationSpeed * 5 : PointLightLocInterpolationSpeed;
	// PL_I_I_S => Point Light Intensity Interpolation Speed
	const float P_L_I_I_S = InbImmediate ? PointLightIntensityInterpolationSpeed * 5 : PointLightIntensityInterpolationSpeed;
	
	if (CurrentWeaponLocation == BottomWeaponLocation && CurrentLightLocation == BottomPointLightLocation
		&& CurrentPointLightIntensity == 0.0f)
	{
		this->GetWorldTimerManager().ClearTimer(this->TimerHandle_LowerWeapons);
		this->StopLoweringWeapon(InbImmediate);
	}
	
	const float DeltaSeconds = this->GetWorld()->GetDeltaSeconds();
	this->CurrentWeaponLocation = UKismetMathLibrary::VInterpTo_Constant(
		this->CurrentWeaponLocation, this->BottomWeaponLocation, DeltaSeconds, L_W_I_S);
	this->CurrentLightLocation = UKismetMathLibrary::VInterpTo_Constant
		(this->CurrentLightLocation, this->BottomPointLightLocation, DeltaSeconds, P_L_L_I_S);
	this->CurrentPointLightIntensity = UKismetMathLibrary::FInterpTo_Constant(
		this->CurrentPointLightIntensity, 0.0f, DeltaSeconds, P_L_I_I_S);
	
	for (AWeapon* Weapon : this->BoxWeaponsArray)
	{
		if (Weapon != nullptr)
		{
			Weapon->SetActorLocation(this->CurrentWeaponLocation);
		}
	}
	
	this->PointLightComp->SetIntensity(this->CurrentPointLightIntensity);
	this->PointLightComp->SetRelativeLocation(this->CurrentLightLocation);
}

// Called to stop lowering the weapons
void AMysteryBoxBase::StopLoweringWeapon(const bool InbImmediate)
{
	if (this->SelectedWeapon != nullptr && this->SelectedWeapon->GetBoxComponent() != nullptr)
	{
		this->SelectedWeapon->GetBoxComponent()->SetCollisionResponseToChannel
			(ECC_GameTraceChannel1, ECR_Ignore);
		this->SelectedWeapon->GetBoxComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("CloseBox"), InbImmediate);
	
	this->GetWorldTimerManager().SetTimer(this->TimerHandle_CloseBox, TimerDelegate, 0.02f, true);
}

// Called to close the mystery box
void AMysteryBoxBase::CloseBox(const bool InbImmediate)
{
	/* Closing mystery box different parts speeds based on if it is an immediate close or not */

	// C_T_B_L_I_S => Close Top Box Location Interpolation Speed
	const float C_T_B_L_I_S = InbImmediate ? CloseTopBoxLocationInterpolationSpeed * 5 : CloseTopBoxLocationInterpolationSpeed;
	// C_T_B_R_I_S => Close Top Box Rotation Interpolation Speed
	const float C_T_B_R_I_S = InbImmediate ? CloseTopBoxRotationInterpolationSpeed * 5 : CloseTopBoxRotationInterpolationSpeed;
	
	if (CurrentTopBoxLocation == FVector(0.0f, 0.0f, 0.0f) &&
		CurrentTopBoxRotation == FRotator(0.0f, 0.0f, 0.0f))
	{
		this->GetWorldTimerManager().ClearTimer(this->TimerHandle_CloseBox);
		this->StopClosingBox();	
	}
	
	const float DeltaSeconds = this->GetWorld()->GetDeltaSeconds();
	this->CurrentTopBoxLocation = UKismetMathLibrary::VInterpTo_Constant(this->CurrentTopBoxLocation,
		FVector(0.0f, 0.0f, 0.0f), DeltaSeconds, C_T_B_L_I_S);
	this->CurrentTopBoxRotation = UKismetMathLibrary::RInterpTo_Constant(this->CurrentTopBoxRotation,
		FRotator(0.0f, 0.0f, 0.0f), DeltaSeconds, C_T_B_R_I_S);
	
	this->AnimatedSceneComp->SetRelativeLocation(this->CurrentTopBoxLocation);
	this->AnimatedSceneComp->SetRelativeRotation(this->CurrentTopBoxRotation);
}

// Called to stop closing the mystery box
void AMysteryBoxBase::StopClosingBox()
{
	this->IsOpen = false;

	if (!this->NotifiedAfterCloseToDeactivate) return;

	this->NotifiedAfterCloseToDeactivate = false;
	
	if (AMainGameState* MainGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(this)))
	{
		MainGameState->DeactivateMysteryBox();
	}
}


/* Getters */

// Method returns whether the mystery box is open or not
bool AMysteryBoxBase::IsMysteryBoxOpen() const
{
	return this->IsOpen;
}

// Method returns whether the mystery box is active or not
bool AMysteryBoxBase::IsMysteryBoxActive() const
{
	return this->IsActive;
}

// Method returns the weapons inside the mystery box
TArray<AWeapon*> AMysteryBoxBase::GetBoxWeaponsArray() const
{
	return this->BoxWeaponsArray;
}