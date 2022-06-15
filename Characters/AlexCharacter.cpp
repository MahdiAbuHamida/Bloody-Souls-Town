// Mahdi Abu Hamida - Final University Project (2022)

#include "AlexCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/AudioComponent.h"
#include "Camera/CameraComponent.h"
#include "BloodySoulsTown/PlayerComponents/AimAssist/AimAssistComponent.h"
#include "BloodySoulsTown/PlayerComponents/Inventory/Inventory.h"
#include "BloodySoulsTown/PlayerComponents/EquippedWeapons/WeaponsSelectionWheel.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "BloodySoulsTown/Other/MainGameMode.h"
#include "BloodySoulsTown/Other/MainGameState.h"
#include "BloodySoulsTown/Other/MyGameInstance.h"
#include "BloodySoulsTown/Other/MySaveGame.h"
#include "BloodySoulsTown/InteractableActors/Interactable.h"
#include "BloodySoulsTown/InteractableActors/Items/AmmoItem/AmmoItem.h"
#include "BloodySoulsTown/InteractableActors/Items/Weapons/Gun.h"
#include "BloodySoulsTown/InteractableActors/Doors/DoorKeyRequired.h"
#include "BloodySoulsTown/InteractableActors/WallWeaponBuy/WallWeaponBuy.h"
#include "BloodySoulsTown/InteractableActors/MysteryBox/MysteryBoxBase.h"
#include "BloodySoulsTown/InteractableActors/PowerStones/PowerStone.h"
#include "BloodySoulsTown/InteractableActors/CashActor/CashActor.h"
#include "BloodySoulsTown/InteractableActors/Obstacle/Obstacle.h"
#include "BloodySoulsTown/InteractableActors/Items/ItemObjects/AmmoItemObject.h"
#include "Sound/SoundCue.h"
#include "BloodySoulsTown/Characters/Animations/PlayerDeathActor.h"
#include "BloodySoulsTown/PlayerComponents/QuestHandler/QuestHandler.h"
#include "BloodySoulsTown/InteractableActors/Items/QuestItem/QuestItem.h"
#include "BloodySoulsTown/InteractableActors/QuestItemsCraftingTable/QuestItemsCraftingTable.h"
#include "BloodySoulsTown/Other/MainPlayerState.h"
#include "BloodySoulsTown/PlayerComponents/Inventory/QuestInventory/QuestInventory.h"
#include "BloodySoulsTown/PlayerComponents/QuestHandler/ShieldActorComponent.h"


#define OUT

// Sets default values
AAlexCharacter::AAlexCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creating heart beat audio component
	this->HeartBeatAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Heart Beat Audio Component"));
	// Attach heart beat audio component to the root component
	this->HeartBeatAudioComp->SetupAttachment(this->RootComponent);
	// Setting the initial volume multiplier of the heart beat audio component
	this->HeartBeatAudioComp->SetVolumeMultiplier(this->CurrentHeartBeatVolMult);

	// Creating take damage voice audio component
	this->TakeDamageVoiceAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Take Damage Voice Audio Component"));
	// Attach take damage voice audio component to the root component
	this->TakeDamageVoiceAudioComp->SetupAttachment(this->RootComponent);

	// Creating aim assist component
	this->AimAssistComp = CreateDefaultSubobject<UAimAssistComponent>(TEXT("AimAssistComp"));

	// Creating AI perception stimuli source component
	this->PerceptionStimuliSourceComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliSource"));

	// Player current health = his full health
	this->CurrentHealth = this->FullHealth;

	this->WeaponSlots.SetNum(4);
	
	// Loading the player mouse/ gamepad settings
	this->LoadPlayerSettings();
}

// Called when the game starts or when spawned
void AAlexCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Setting the player character maximum walk speed to be normal on begin play
	this->GetCharacterMovement()->MaxWalkSpeed = NormalWalkingSpeed;

	// Assigning a pointer reference to the main game mode
	this->MainGameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this));
	// Assigning a pointer reference to the main game instance
	this->MyGameInstance = Cast<UMyGameInstance>(this->GetGameInstance());
	// Assigning a pointer reference to the main game state
	this->MainGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(this));
	// Assigning a pointer reference to the player controller
	this->PlayerController = Cast<APlayerController>(this->GetWorld()->GetFirstPlayerController());
	// Assigning a pointer reference to the player state
	this->MainPlayerState = Cast<AMainPlayerState>(this->GetPlayerState());

	// Declaring an array of skeletal meshes components
	TArray<USkeletalMeshComponent*> MeshArray;
	// Adding all skeletal meshes found in the player character components structure to the array
	this->GetComponents<USkeletalMeshComponent>(MeshArray);
	
	/*
	  Only if the skeletal meshes found were exactly 2 as it has to be,
	  then assigning a pointer to the main player character skeletal mesh
	 */
	if (MeshArray.Num() == 2)
	{
		this->PlayerMesh = MeshArray[1];
	}

	// Assigning a pointer reference to the player camera manager actor
	this->PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	// Assigning a pointer reference to the player character camera component
	this->PlayerCamera = Cast<UCameraComponent>(this->GetComponentByClass(UCameraComponent::StaticClass()));

	// Assigning a pointer reference to the player inventory component
	this->Inventory = FindComponentByClass<UInventory>();
	// Assigning a pointer reference to the player weapons selection wheel component
	this->WeaponsWheel = FindComponentByClass<UWeaponsSelectionWheel>();
	// Assigning a pointer reference to the player quest inventory component
	this->QuestInventory = FindComponentByClass<UQuestInventory>();
	// Assigning a pointer reference to the player quest handler component
	this->QuestHandlerComp = FindComponentByClass<UQuestHandler>();
	// Setting the player character current health to be full
	this->CurrentHealth = this->FullHealth;

	/* Saving & Loading player walk-through gameplay has been cancelled */
	// Loading the player in-game properties
	// this->LoadPlayerProperties();

	// Check if the begin play weapon class is valid (assigned in blueprint child class of this)
	if (IsValid(this->BeginPlayWeaponClass))
	{
		// If it's valid then spawn a weapon of type begin play weapon class, in world
		if (AWeapon* Weapon = Cast<AWeapon>(this->GetWorld()->SpawnActor(this->BeginPlayWeaponClass)))
		{
			// Setting the spawned weapon to be hidden in world, invisible
			Weapon->SetActorHiddenInGame(true);
			// Setting the spawned weapon enabled collision to be no collision
			Weapon->SetActorEnableCollision(ECollisionEnabled::NoCollision);
			// Assigning the item to be picked up to the spawned weapon
			this->ItemToPickUp = Weapon;
			/*
			  Called to add the spawned weapon to the player character hands
			  as well as adding it to inventory & selection wheel components of the player character
			 */
			this->InteractItem();
		}
	}
	
	/*
	  Setting a timer loop to show/ hide interactable actors interaction widgets
	  using a line trace which will run about 14 times a second
	 */
	this->GetWorldTimerManager().SetTimer(this->TimerHandle_LookForInteractableActor,
		this, &AAlexCharacter::LookForInteractableActorInteractionWidget, 0.07f, true);
}

// Called once in the construct to load the player custom control-settings
void AAlexCharacter::LoadPlayerSettings()
{
	if (!UGameplayStatics::DoesSaveGameExist("PlayerCustomSettings", 0)) return;
	
	UMySaveGame* MySaveGame = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot("PlayerCustomSettings", 0));
	if (MySaveGame == nullptr) return;
	
	/*
	  Loading player's Gamepad/ Mouse settings
	*/
	this->GamepadSensitivityX = MySaveGame->ControllerSenX;
	this->GamepadSensitivityY = MySaveGame->ControllerSenY;
	this->GamepadLowZoomSenMultiplier = MySaveGame->LowZoomAdsControllerSenModifier;
	this->GamepadHighZoomSenMultiplier = MySaveGame->HighZoomAdsControllerSenModifier;
	
	// this->MouseSensitivityX = MySaveGame->MouseSenX;
	// this->MouseSensitivityY = MySaveGame->MouseSenY;
	// this->MouseLowZoomSenMultiplier = MySaveGame->LowZoomAdsMouseSenModifier;
	// this->MouseHighZoomSenMultiplier = MySaveGame->HighZoomAdsMouseSenModifier;
}

/***
 * (save/ load game system has been cancelled for some technical reasons)
 ***/

/* function won't be called anywhere */
// Called once in begin play func to load player's properties
void AAlexCharacter::LoadPlayerProperties()
{
	if (UGameplayStatics::DoesSaveGameExist("MySlot", 0))
	{
		UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot("MySlot", 0));
		if (SaveGameInstance != nullptr)
		{
			this->CurrentHealth = SaveGameInstance->PlayerHealth;
			this->ItemObjectOnUse = SaveGameInstance->CurrentItemObjectInUse;
			for (int8 i = 0; i < SaveGameInstance->PlayerWeaponsSlots.Num(); i++)
			{
				this->WeaponSlots[i] = SaveGameInstance->PlayerWeaponsSlots[i];
				++i;
			}
			this->CurrentWeaponSlotIndex = SaveGameInstance->CurrentWeaponSlotIndex;
			if (this->CurrentWeaponSlotIndex > -1)
			{
				this->WeaponClassOnUse = this->ItemObjectOnUse->GetItemClass();
				this->SpawnWeaponInHands();
			}
		}
		this->LoadPowerStones(SaveGameInstance);
	}
	else if (this->WeaponsWheel != nullptr)
	{
		if (AWeapon* Weapon = Cast<AWeapon>(this->GetWorld()->SpawnActor(this->BeginPlayWeaponClass)))
		{
			Weapon->SetActorHiddenInGame(true);
			Weapon->SetActorEnableCollision(ECollisionEnabled::NoCollision);
			this->ItemToPickUp = Weapon;
			this->InteractItem();
		}
	}
}

/* function won't be called anywhere */
/*
  Called once in begin play func to load player's power stones
  and connect them with the same power stones actors in the world
 */
void AAlexCharacter::LoadPowerStones(const UMySaveGame* SaveGameInstance)
{
	TArray<FName> Keys;
	SaveGameInstance->PowerStones.GetKeys(Keys);
	if (Keys.Num() == 0) return;
	
	TArray<AActor*> PowerStonesInMap;
	UGameplayStatics::GetAllActorsOfClass(this, APowerStone::StaticClass(), PowerStonesInMap);

	APowerStone* Speedy; APowerStone* DoubleSouls;
	APowerStone* RapidFire; APowerStone* BigShoulders;

	for (AActor* Actor : PowerStonesInMap)
	{
		if (APowerStone* PowerStone = Cast<APowerStone>(Actor))
		{
			switch (PowerStone->GetPowerStoneType())
			{
				case PowerStone_Speedy: {Speedy = PowerStone; break;}
				case PowerStone_DoubleSouls: {DoubleSouls = PowerStone; break;}
				case PowerStone_RapidFire: 	{RapidFire = PowerStone; break;}
				case PowerStone_BigShoulders: {BigShoulders = PowerStone; break;}
				default: ;
			}
		}
	}
	
	for (const FName Key : Keys)
	{
		if (SaveGameInstance->PowerStones[Key])
		{
			if (Key == "Speedy")
			{
				this->MaximumWalkingSpeed *= this->SpeedyPowerStonePowerMultiplier;
				this->NormalWalkingSpeed *= this->SpeedyPowerStonePowerMultiplier;
				if (this->CurrentWeapon != nullptr)
				{
					this->MaximumWalkingSpeedWithWeapon *= this->SpeedyPowerStonePowerMultiplier;
					this->NormalWalkingSpeedWithWeapon *= this->SpeedyPowerStonePowerMultiplier;
				}
				this->PowerStonesByTypes.AddUnique(PowerStone_Speedy);
				this->PowerStones.AddUnique(Speedy);
				this->OnEquipPowerStoneDel.Broadcast();
			}
			else if (Key == "DoubleSouls")
			{
				this->FullHealth = FMath::Min(this->FullHealth * 2, this->DoubleSoulsFullHealth);
				this->PowerStonesByTypes.AddUnique(PowerStone_DoubleSouls);
				this->PowerStones.AddUnique(DoubleSouls);
				this->CurrentHealth = this->FullHealth;
				this->OnPlayerCurrentHealthChanged.Broadcast();
				this->OnEquipPowerStoneDel.Broadcast();
			}
			else if (Key == "RapidFire")
			{
				this->PowerStonesByTypes.AddUnique(PowerStone_RapidFire);
				this->PowerStones.AddUnique(RapidFire);
				if (Cast<AGun>(this->CurrentWeapon) != nullptr)
				{
					this->CurrentWeapon->SetDelayBetweenRapidAttacks();
				}
				if (Cast<AGun>(this->LeftHandCurrentWeapon) != nullptr)
				{
					this->LeftHandCurrentWeapon->SetDelayBetweenRapidAttacks();
				}
				this->OnEquipPowerStoneDel.Broadcast();
			}
			else if (Key == "BigShoulders")
			{
				this->PowerStonesByTypes.AddUnique(PowerStone_BigShoulders);
				this->PowerStones.AddUnique(BigShoulders);
				this->Inventory->Columns *= BigShoulders->GetPowerValue();
				this->Inventory->Rows *= BigShoulders->GetPowerValue();
				this->Inventory->OnRowsColumnsChangedDel.Broadcast();
				this->OnEquipPowerStoneDel.Broadcast();
			}
		}
	}
}

// Called every frame
void AAlexCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*
	  Check if both the aim assist component & the player controller are not a null pointers
	  and that the last input by the player was a gamepad key and has a weapon in hands
	 */
	if (this->AimAssistComp != nullptr && this->PlayerController != nullptr
		&& this->bIsGamepadKey && this->CurrentWeapon != nullptr)
	{
		// Declaring 2 floats to be assigned with the gamepad right stick x & y axis values  
		float RightStickX = 0, RightStickY = 0;

		/*
		  Getting the gamepad right stick state (x & y axis values)
		  and assigning them to RightStickX, RightStickY
		 */
		this->PlayerController->GetInputAnalogStickState(EControllerAnalogStick::CAS_RightStick,
			RightStickX, RightStickY);
		RightStickX = FMath::Abs(RightStickX);
		RightStickY = FMath::Abs(RightStickY);

		// Check if the player is moving right stick at least 10%
		if (RightStickX >= 0.1f || RightStickY >= 0.1f)
		{
			float AimAssistPower;
			// Check if the player is aiming
			if (this->bIsAiming)
			{
				/*
				  The player is currently aiming with a weapon, starting aim assist
				  with a higher speeds (stronger aim assist)
				 */
				RightStickX = UKismetMathLibrary::MapRangeClamped(RightStickX, 0.1f, 1.f,
					0.5f, 0.05f);
				RightStickY = UKismetMathLibrary::MapRangeClamped(RightStickY, 0.1f, 1.f,
					0.5f, 0.05f);
				
				AimAssistPower = (RightStickX + RightStickY) / 2;
				this->AimAssistComp->StartAimAssist(InterpolationSpeedForMovingAimWhileAds + AimAssistPower,
					FMath::Max(1500.0f, this->CurrentWeapon->GetWeaponAttackRange() / 2));
			}
			else
			{
				/*
				  the player is currently not aiming down sights, starting aim assist
				  with a slower speeds (slightly strong aim assist)
				 */
				RightStickX = UKismetMathLibrary::MapRangeClamped(RightStickX, 0.1f, 1.f,
					0.1f, 0.01f);
				RightStickY = UKismetMathLibrary::MapRangeClamped(RightStickY, 0.1f, 1.f,
					0.1f, 0.01f);
				
				AimAssistPower = (RightStickX + RightStickY) / 2;
				this->AimAssistComp->StartAimAssist(this->InterpolationSpeedForMovingAim + AimAssistPower,
					FMath::Max(1500.0f, this->CurrentWeapon->GetWeaponAttackRange() / 2));
			}
		}
	}

	/*
	  Check if the last input by the player was a gamepad key
	  and if the left stick y axis (moving forward) was smaller than the minimum value to allow sprinting
	 */
	if (this->bIsGamepadKey && this->ForwardAxisValue < this->MaximumForwardAxisValue)
	{
		// Called to stop sprinting
		SprintReleased();
	}
	/*
	  Otherwise, check first if the last input by the player was a gamepad key
	  if true, means that the forward axis value gotten from the move forward function update
	  is smaller than the minimum forward axis value (min val that allows dynamic sprint)
	  & check if the current delta time seconds in the world minus
	  the delta time seconds when the last sprint happened using a gamepad is more than
	  the current delay of releasing the dynamic sprint then release the sprint
	  as it'll release it as long as the forward axis value is smaller than the minimum
	 */
	else if (this->bIsGamepadKey && GetWorld()->GetTimeSeconds() - LastSprintOnGamepad > SprintSwitchInterruption)
	{
		// Called to stop sprinting
		SprintReleased();
	}
	
	// Called to update the blood screen every frame
	this->UpdateBloodScreen();

	// Check if regaining health is allowed (means player character has taken a damage)
	if (this->bRegainHealth && !this->bDied)
	{
		// validating that the player character current health < full health
		if (this->CurrentHealth < this->FullHealth)
		{
			// Adding health to the player character with a maximum equal to the full health
			this->CurrentHealth = FMath::Min(this->CurrentHealth + this->HealthRegainStep, this->FullHealth);
			/*
			  Broadcasting to the health widget class to change
			  the heart beat speed & color material according to the new current health value
			 */
			this->OnPlayerCurrentHealthChanged.Broadcast();
			// Check if the heart beat audio component is not a null pointer
			if (this->HeartBeatAudioComp != nullptr)
			{
				/*
				  Check if the player character current health is bigger
				  to the maximum health value that allows showing blood screen material
				 */
				if (this->CurrentHealth > this->MaximumHealthToShowBloodScreen)
				{
					// Check if the heart beat audio component is playing
					if (this->HeartBeatAudioComp->IsPlaying())
					{
						// Stopping the heart beat audio component from playing sfx
						this->HeartBeatAudioComp->Stop();
						// Setting the heart beat audio component volume multiplier to 0.001 (~ 0)
						this->HeartBeatAudioComp->VolumeMultiplier = 0.001f;
					}
				}
				/*
				  Otherwise means the player character health is considered
				  as a low health (danger health zone), then check if the heart beat
				  audio component volume multiplier is still bigger or equal to minimum step (0.002)
				 */
				else if (this->CurrentHeartBeatVolMult >= 0.002f)
				{
					/*
					  Decreasing the heart beat audio component volume multiplier by one step value (0.002)
					  and setting it to the new value with a minimum value of 0.001 (~ 0)
					 */
					this->CurrentHeartBeatVolMult = FMath::Max(0.001f, this->CurrentHeartBeatVolMult - 0.002f);
					// Setting the final result to the heart beat audio component volume multiplier
					this->HeartBeatAudioComp->SetVolumeMultiplier(this->CurrentHeartBeatVolMult);
				}
			}
		}
		// Otherwise means the player character current health is equal to the full health value
		else
		{
			// Setting regain health to false
			this->bRegainHealth = false;
		}
	}
}

// Called to bind functionality to input
void AAlexCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
	/*** Movements functionality binding to inputs ***/
	
	// Player move forward/ backward functionality binding to input
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AAlexCharacter::MoveForward);
	
	// Player move right/ left functionality binding to input
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AAlexCharacter::MoveRight);
	
	// Player look right/ left functionality binding to input (mouse)
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &AAlexCharacter::LookRight);
	
	// Player look up/ down functionality binding to input (mouse)
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AAlexCharacter::LookUp);

	// Player look right/ left functionality binding to input (gamepad)
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AAlexCharacter::LookRightRate);
	
	// Player look up/ down functionality binding to input (gamepad)
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AAlexCharacter::LookUpRate);

	// Player active sprint functionality binding to input
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &AAlexCharacter::SprintPressed);
	
	// Player sprint release functionality binding to input
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &AAlexCharacter::SprintReleased);
	

	/* Actions functionality binding to inputs */

	// Player interaction functionality binding to input
	PlayerInputComponent->BindAction(TEXT("Interact"), EInputEvent::IE_Pressed, this, &AAlexCharacter::Interact);

	// Player release interaction functionality binding to input
	PlayerInputComponent->BindAction(TEXT("Interact"), EInputEvent::IE_Released, this, &AAlexCharacter::ReleaseInteract);
	
	// Player start attacking (1st attack input press) functionality binding to input
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AAlexCharacter::StartAttack);
	
	// Player stop attacking functionality binding to input
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Released, this, &AAlexCharacter::StopAttack);
	
	// Player gun reloading functionality binding to input
	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &AAlexCharacter::Reload);
	
	/*
	  Player right hand weapon attack (mouse - dual weapons)/
	  left hand weapon attack (gamepad - dual weapons) functionality binding to input
	 */
	PlayerInputComponent->BindAction(TEXT("ADS"), EInputEvent::IE_Pressed, this, &AAlexCharacter::AdsStartAttack);
	
	/*
	  Player right hand weapon *stop* attack (mouse - dual weapons)/
	  left hand weapon *stop* attack (gamepad - dual weapons) functionality binding to input
	 */
	PlayerInputComponent->BindAction(TEXT("ADS"), EInputEvent::IE_Released, this, &AAlexCharacter::AdsStopAttack);

	
	/* Swapping between weapons using keyboard functionality binding to inputs */
	
	PlayerInputComponent->BindAction(TEXT("WeaponSlot_01"), EInputEvent::IE_Pressed, this, &AAlexCharacter::UseWeaponSlot1);
	PlayerInputComponent->BindAction(TEXT("WeaponSlot_02"), EInputEvent::IE_Pressed, this, &AAlexCharacter::UseWeaponSlot2);
	PlayerInputComponent->BindAction(TEXT("WeaponSlot_03"), EInputEvent::IE_Pressed, this, &AAlexCharacter::UseWeaponSlot3);
	PlayerInputComponent->BindAction(TEXT("WeaponSlot_04"), EInputEvent::IE_Pressed, this, &AAlexCharacter::UseWeaponSlot4);

	// Swapping to next/ first weapon using mouse scroll wheel up or gamepad functionality binding to inputs
	PlayerInputComponent->BindAction(TEXT("NextWeapon"), EInputEvent::IE_Pressed, this, &AAlexCharacter::UseNextWeapon);
	// Swapping to previous/ last weapon using mouse scroll wheel down functionality binding to inputs
	PlayerInputComponent->BindAction(TEXT("PreviousWeapon"), EInputEvent::IE_Pressed, this, &AAlexCharacter::UsePreviousWeapon);
}


/* Movements functions */

// Called to update the player character forward vector value (move forward or backwards)
void AAlexCharacter::MoveForward(float AxisValue)
{
	/*
	  If the character was moving backwards (AxisValue is negative), CanCharacterRun set to false
	  or if the character was aiming with a gun
	 */
	this->bCanCharacterRun = AxisValue >= 0 && !this->bIsAiming;

	/*
	  Check if last input by the player was a gamepad key
	 */
	if (this->bIsGamepadKey)
	{
		// update the forward axis value
		this->ForwardAxisValue = AxisValue;
	}

	// Updating the the player character forward vector 
	this->AddMovementInput(GetActorForwardVector() * AxisValue);

	
	/* Playing camera shakes */

	// Declaring a float with a value of the velocity length of the player character
	const float Speed = this->GetVelocity().Size();
	// Declaring a vector with a value of the player character current location
	const FVector Location = this->GetActorLocation();

	// Check if the ads camera shake class is valid & the player is aiming
	if (IsValid(this->AdsCameraShake) && this->bIsAiming)
	{
		// Playing the ads camera shake in the player character location
		UGameplayStatics::PlayWorldCameraShake(this->GetWorld(), this->AdsCameraShake, Location, 100.f, 0.f);
	}
	// Otherwise, Check if the idle camera shake class is valid & the player character speed is nearly equal to 0
	else if (IsValid(this->IdleCameraShakeClass) && FMath::IsNearlyEqual(Speed, 0.0f, 0.1f))
	{
		// Playing the idle camera shake in the player character location
		UGameplayStatics::PlayWorldCameraShake(this->GetWorld(), this->IdleCameraShakeClass, Location, 100.f, 0.f);
	}
	/*
	  Otherwise, Check if the walking camera shake is valid & the player character
	  speed is nearly equal to the current normal walking speed value
	 */
	else if (IsValid(this->WalkingCameraShakeClass) && FMath::IsNearlyEqual(Speed, this->NormalWalkingSpeed, 0.1f))
	{
		// Playing the walking camera shake in the player character location
		UGameplayStatics::PlayWorldCameraShake(this->GetWorld(), this->WalkingCameraShakeClass, Location, 100.f, 0.f);
	}
	/*
	  Otherwise, Check if the running camera shake class is valid & the player character
	  speed is higher than the current normal walking speed
	 */
	else if (IsValid(this->RunningCameraShakeClass) && Speed > this->NormalWalkingSpeed)
	{
		// Playing the running camera shake in the player character location
		UGameplayStatics::PlayWorldCameraShake(this->GetWorld(), this->RunningCameraShakeClass, Location, 100.f, 0.f);
	}

	// Updating the value that holds whether the player is near a wall or not
	this->CheckIfNearWall();

	// Check player velocity for activating footsteps effects
	this->CheckVelocityForFootsteps();
}

// Called to update the player right vector value (move right or left)
void AAlexCharacter::MoveRight(float AxisValue)
{
	// Updating the the player character right vector 
	this->AddMovementInput(GetActorRightVector() * AxisValue);

	// Updating the value that holds whether the player is near a wall or not
	this->CheckIfNearWall();

	// Check player velocity for activating footsteps effects
	this->CheckVelocityForFootsteps();
}

// Called to update the player character pitch control rotation (mouse y-axis)
void AAlexCharacter::LookUp(float AxisValue)
{
	/*
	  Check if the player is aiming & with a gun (not melee weapon)
	  to add pitch input according to the axis value & mouse sen y / 20 & mouse high/ low zoom sen multiplier 
	 */
	if (this->bIsAiming && this->CurrentWeapon != nullptr && this->CurrentWeapon->GetWeaponType() != Weapon_Melee)
	{
		// Check if the current weapon type is a sniper
		if (this->CurrentWeapon->GetWeaponType() == Weapon_Sniper)
		{
			// Adding pitch input axis value multiplied by mouse sen y / 20 * mouse [high] zoom sen multiplier
			this->AddControllerPitchInput(AxisValue * (this->MouseSensitivityY/20) * this->MouseHighZoomSenMultiplier);
		}
		// Otherwise means the current weapon type is a gun but not sniper rifle (not high zoom)
		else
		{
			// Adding pitch input axis value multiplied by mouse sen y / 20 * mouse [low] zoom sen multiplier
			this->AddControllerPitchInput(AxisValue * (this->MouseSensitivityY/20) * this->MouseLowZoomSenMultiplier);
		}
	}
	// Otherwise add pitch input axis value without any low/ high zoom sen mult modifications
	else
	{
		// Adding pitch input axis value multiplied by mouse sen y / 20
		this->AddControllerPitchInput(AxisValue * (this->MouseSensitivityY/20));
	}
}

// Called to update the player character yaw control rotation (mouse x-axis)
void AAlexCharacter::LookRight(float AxisValue)
{
	/*
	  Check if the player is aiming & with a gun (not melee weapon)
	  to add yaw input according to the axis value & mouse sen x / 20 & mouse high/ low zoom sen multiplier 
	 */
	if (this->bIsAiming && this->CurrentWeapon != nullptr && this->CurrentWeapon->GetWeaponType() != Weapon_Melee)
	{
		// Check if the current weapon type is a sniper
		if (this->CurrentWeapon->GetWeaponType() == Weapon_Sniper)
		{
			// Adding pitch input axis value multiplied by mouse sen x / 20 * mouse [high] zoom sen multiplier
			this->AddControllerYawInput(AxisValue * (this->MouseSensitivityX/20) * this->MouseHighZoomSenMultiplier);
		}
		// Otherwise means the current weapon type is a gun but not sniper rifle (not high zoom)
		else
		{
			// Adding yaw input axis value multiplied by mouse sen x / 20 * mouse [low] zoom sen multiplier
			this->AddControllerYawInput(AxisValue * (this->MouseSensitivityX/20) * this->MouseLowZoomSenMultiplier);
		}
	}
	// Otherwise add yaw input axis value without any low/ high zoom sen mult modifications
	else
	{
		// Adding yaw input axis value multiplied by mouse sen x / 20
		this->AddControllerYawInput(AxisValue * (this->MouseSensitivityX/20));
	}
}

// Called to update the player character pitch control rotation (gamepad right stick y-axis)
void AAlexCharacter::LookUpRate(float AxisValue)
{
	/*
	  Check if the player is aiming & with a gun (not melee weapon)
	  to add pitch input according to the axis value & gamepad sen y & gamepad high/ low zoom sen multiplier 
	 */
	if (this->bIsAiming && this->CurrentWeapon != nullptr && this->CurrentWeapon->GetWeaponType() != Weapon_Melee)
	{
		// Check if the current weapon type is a sniper
		if (this->CurrentWeapon->GetWeaponType() == Weapon_Sniper)
		{
			/*
			  Adding pitch input axis value multiplied by gamepad sen y *
			  (gamepad [high] zoom sen multiplier / 4) * frame delta time seconds
			 */
			this->AddControllerPitchInput(AxisValue * this->GamepadSensitivityY * (this->GamepadHighZoomSenMultiplier/4)
				* this->GetWorld()->GetDeltaSeconds());
		}
		// Otherwise means the current weapon type is a gun but not sniper rifle (not high zoom) 
		else
		{
			/*
			  Adding pitch input axis value multiplied by gamepad sen y *
			  (gamepad [low] zoom sen multiplier / 2) * frame delta time seconds
			 */
			this->AddControllerPitchInput(AxisValue * this->GamepadSensitivityY * (this->GamepadLowZoomSenMultiplier/2)
				* this->GetWorld()->GetDeltaSeconds());
		}
	}
	// Otherwise add pitch input axis value without any low/ high zoom sen mult modifications
	else
	{
		// Adding pitch input axis value multiplied by gamepad sen y * frame delta time seconds
		this->AddControllerPitchInput(AxisValue * this->GamepadSensitivityY * GetWorld()->GetDeltaSeconds());
	}
}

// Called to update the player character yaw control rotation (gamepad right stick x-axis)
void AAlexCharacter::LookRightRate(float AxisValue)
{
	/*
	  Check if the player is aiming & with a gun (not melee weapon)
	  to add yaw input according to the axis value & gamepad sen x & gamepad high/ low zoom sen multiplier 
	 */
	if (this->bIsAiming && this->CurrentWeapon != nullptr && this->CurrentWeapon->GetWeaponType() != Weapon_Melee)
	{
		// Check if the current weapon type is a sniper
		if (this->CurrentWeapon->GetWeaponType() == Weapon_Sniper)
		{
			/*
			  Adding yaw input axis value multiplied by gamepad sen x *
			  (gamepad [high] zoom sen multiplier / 4) * frame delta time seconds
			 */
			this->AddControllerYawInput(AxisValue * this->GamepadSensitivityX * (this->GamepadHighZoomSenMultiplier/4)
				* GetWorld()->GetDeltaSeconds());
		}
		// Otherwise means the current weapon type is a gun but not sniper rifle (not high zoom)
		else
		{
			/*
			  Adding pitch input axis value multiplied by gamepad sen x *
			  (gamepad [low] zoom sen multiplier / 2) * frame delta time seconds
			 */
			this->AddControllerYawInput(AxisValue * this->GamepadSensitivityX * (this->GamepadLowZoomSenMultiplier/2)
				* GetWorld()->GetDeltaSeconds());
		}
	}
	// Otherwise add yaw input axis value without any low/ high zoom sen mult modifications
	else
	{
		// Adding yaw input axis value multiplied by gamepad sen x * frame delta time seconds
		this->AddControllerYawInput(AxisValue * this->GamepadSensitivityX * GetWorld()->GetDeltaSeconds());
	}
}

// Called to check if the player is near a wall (mesh) - to prevent the weapons from going in a wall
void AAlexCharacter::CheckIfNearWall()
{
	// If the player camera or the current weapon is a nullptr then return
	if (this->PlayerCamera == nullptr || this->CurrentWeapon == nullptr) return;

	/*
	  Doing a single collision trace starting from the player camera location in world space
	  ending in a maximum location of the player camera location in world space +
	  forward vector of the player camera * 80, hitting the first object of a world static collision channel only
	  as well as passing the hit result to the Hit & getting the returned boolean
	  result if there was a hit assigning it to bIsNearWall 
	 */
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (this->CurrentWeapon != nullptr)
	{
		Params.AddIgnoredActor(this->CurrentWeapon);
	}
	
	this->bIsNearWall = this->GetWorld()->LineTraceSingleByChannel(Hit, this->PlayerCamera->GetComponentLocation(),
		this->PlayerCamera->GetComponentLocation() + this->PlayerCamera->GetForwardVector() * 80,
		ECC_Visibility, Params);
}

// Called to update the player character velocity
void AAlexCharacter::UpdateCharacterVelocity()
{
	// Check if the current player character max walk speed is equal to the target velocity
	if (this->GetCharacterMovement()->MaxWalkSpeed == this->TargetVelocity)
	{
		// Clearing the timer which handles to updates the player character velocity
		this->GetWorldTimerManager().ClearTimer(this->TimerHandle_UpdateCharacterVelocity);
		return;
	}
	// Check if the update step was equal to 1 (to add plus 1 instead of minus)
	if (this->TargetVelocityUpdateStep == 1)
	{
		// Changing the player character maximum speed by increasing it with 1 
		this->GetCharacterMovement()->MaxWalkSpeed = FMath::Min(this->TargetVelocity, this->GetCharacterMovement()->MaxWalkSpeed + 1);
	}
	// Otherwise means update step is equal to -1
	else
	{
		// Changing the player character maximum speed by decreasing it with 1
		this->GetCharacterMovement()->MaxWalkSpeed = FMath::Max(this->TargetVelocity, this->GetCharacterMovement()->MaxWalkSpeed - 1);
	}
}


/*******************************************************/


/* Player actions functions */

/*
  Called when the player hit once the button (gamepad face button up)/ mouse scroll wheel up
  to swap to the next weapon slot (or back to slot 1 - or none)
 */
void AAlexCharacter::UseNextWeapon()
{
	// Check if the current weapon slot index is invalid then return
	if (this->WeaponSlots.Num() < 1 || this->CurrentWeaponSlotIndex < 0
		|| this->CurrentWeaponSlotIndex > this->WeaponSlots.Num() - 1) return;
	// The current weapon slot index is valid
	
	int8 i, Iterator = 0;
	/*
	  If current weapon slot index was the last slot (3) then set i to equal to 0
	  else then set i to be equal to the current weapon slot index plus 1 
	 */
	this->CurrentWeaponSlotIndex == this->WeaponSlots.Num() - 1 ? i = 0 : i = this->CurrentWeaponSlotIndex + 1;
	while (Iterator < this->WeaponSlots.Num() * 2)
	{
		++Iterator;
		// Making sure than the i value doesn't reach more than the max weapon slots index value
		if (i >= WeaponSlots.Num())
		{
			i = 0;
		}
		/*
		  Otherwise, Check if i reached the current weapon slot index value
		  that means it is the only slot with a weapon, return
		 */
		else if (i == this->CurrentWeaponSlotIndex) return;
		
		// Check if the weapon object in index i is not a null
		if (WeaponSlots[i] != nullptr)
		{ // Calling the right function to use weapon slot #i+1
			switch (i)
			{
				case 0: this->UseWeaponSlot1(); break;
				case 1: this->UseWeaponSlot2(); break;
				case 2: this->UseWeaponSlot3(); break;
				case 3: this->UseWeaponSlot4();
				default: ;
			}
			// Next weapon slot has been found successfully, return 
			return;
		}
		++i;
	}
}

/*
  Called when the player hit once the button mouse scroll wheel down
  to swap to the previous weapon slot (or back in reverse to slot 4 - or none)
 */
void AAlexCharacter::UsePreviousWeapon()
{
	// Check if the current weapon slot index is invalid then return
	if (this->WeaponSlots.Num() < 1 || this->CurrentWeaponSlotIndex < 0
		|| this->CurrentWeaponSlotIndex > this->WeaponSlots.Num() - 1) return;
	// The current weapon slot index is valid
	
	int8 i, Iterator = 0;
	/*
	  If current weapon slot index was the first slot (0) then set i to equal to array length - 1
	  else then set i to be equal to 0
	 */
	this->CurrentWeaponSlotIndex - 1 == -1 ? i = WeaponSlots.Num() - 1 : i = this->CurrentWeaponSlotIndex - 1;
	while (Iterator < this->WeaponSlots.Num() * 2)
	{
		++Iterator;

		// Making sure than the i value doesn't reach less than 0
		if (i < 0)
		{
			i = WeaponSlots.Num() - 1;
		}
		/*
		  Otherwise, Check if i reached the current weapon slot index value
		  that means it is the only slot with a weapon, return
		 */
		else if (i == this->CurrentWeaponSlotIndex)
		{
			return;
		}
		// Check if the weapon object in index i is not a null
		if (WeaponSlots[i] != nullptr)
		{
			// Calling the right function to use weapon slot #i+1
			switch (i)
			{
				case 0: this->UseWeaponSlot1(); break;
				case 1: this->UseWeaponSlot2(); break;
				case 2: this->UseWeaponSlot3(); break;
				case 3: this->UseWeaponSlot4();
				default: ;
			}
			// Previous weapon slot has been found successfully, return 
			return;
		}
		--i;
	}
}

// Called when the player holds down the sprint button/ key
void AAlexCharacter::SprintPressed()
{
	if (!this->bSprintNotified)
	{
		this->bSprintNotified = true;
	}
	// Check the player character can run   
	if (!this->bCanCharacterRun) return;
	/*
	 Check if the last input by the player was a gamepad key
	 to update values of the dynamic sprint (increasing)
	*/
	if (this->bIsGamepadKey)
	{
		// Increasing 1 to sprint presses counter
		++this->CounterSprintPressed;
		// Increasing initial delay in seconds (delay sprint to non sprint)
		this->SprintSwitchInterruption += this->SwitchIncreaseDelay;
	}

	// Declaring a reference to world timer manager
	FTimerManager &TimerManager = this->GetWorldTimerManager();
	// Check if the timer handle of updating the player character velocity is still active
	if (TimerManager.IsTimerActive(this->TimerHandle_UpdateCharacterVelocity))
	{
		/*
		  It is active so clearing it as the timer is gonna
		  run again from current velocity to new target velocity in a given rate (0.001)
		 */
		TimerManager.ClearTimer(this->TimerHandle_UpdateCharacterVelocity);
	}

	/*
	  Asking if the player has a weapon then get the current
	  max walking speed with weapon, otherwise getting the normal max walking speed
	  (without weapon), two cases assigning the value to a float
	 */
	const float FinalMaximumWalkingSpeed = this->CurrentWeapon != nullptr ? this->MaximumWalkingSpeedWithWeapon
												: this->MaximumWalkingSpeed;
	/*
	  Check if the player character max walk speed is smaller than
	  the [FinalMaximumWalkingSpeed] then set a timer to reach a
	  new 'target walking speed' = [FinalMaximumWalkingSpeed]
	 */
	if (this->GetCharacterMovement()->MaxWalkSpeed < FinalMaximumWalkingSpeed)
	{
		// Assigning the new max walk speed to [TargetVelocity]
		this->TargetVelocity = FinalMaximumWalkingSpeed;
		/*
		  Cause the current max walk speed is smaller than the target
		  speed value, then the [TargetVelocityUpdateStep] should be positive, equal to 1
		 */
		this->TargetVelocityUpdateStep = 1;
		// Setting the timer handle of updating the player character velocity
		TimerManager.SetTimer(this->TimerHandle_UpdateCharacterVelocity, this,
			&AAlexCharacter::UpdateCharacterVelocity, 0.001, true);
	}
	// If sprint released is true, reset it to false
	if (this->IsSprintReleased)
	{
		this->IsSprintReleased = false;
	}
}

// Called when the player releases the sprint button/ key
void AAlexCharacter::SprintReleased()
{
	// When sprint is released, sprint notified resets to false
	this->bSprintNotified = false;

	/*
	  Check if the last input by the player was a gamepad key
	  to update values of the dynamic sprint (decreasing)
	 */
	if (this->bIsGamepadKey)
	{
		// Decreasing the counter of sprint pressed by 1 if it was bigger than 1
		if (this->CounterSprintPressed > 1)
		{
			// This results to decrease the dynamic sprint when using a gamepad
			--this->CounterSprintPressed;
		}
		// This results to decrease the dynamic sprint when using a gamepad
		this->SprintSwitchInterruption -= this->CounterSprintPressed * this->SwitchDecreaseDelay;
		this->LastSprintOnGamepad = GetWorld()->GetTimeSeconds();
		this->IsSprintReleased = true;
		/*
		  Check if the forward input movement axis value is > than the minimum forward axis value that allows
		  gamepad dynamic sprint, return 
		 */
		if (this->ForwardAxisValue >= this->MinimumForwardAxisValue)
		{
			return;
		}
	}

	/*
	  Asking if the current player character max walk speed is < the normal walk speed
	  then the [TargetVelocityUpdateStep] should be positive, equal to 1 (as it goes to normal walk speed)
	  after sprint is released, otherwise the [TargetVelocityUpdateStep] = -1
	 */
	this->GetCharacterMovement()->MaxWalkSpeed < this->NormalWalkingSpeed ? this->TargetVelocityUpdateStep = 1 :
			this->TargetVelocityUpdateStep = -1;

	// Check if current weapon is not a null pointer
	if (this->CurrentWeapon != nullptr)
	{
		// If the player is aiming with the current weapon
		if (this->bIsAiming)
		{
			// Reassign target velocity to current weapon ads walk speed
			this->TargetVelocity = this->CurrentWeapon->PlayerWalkingSpeedAds;
		}
		/*
		  Otherwise means the player is not aiming, so set the target velocity to
		  the current weapon normal walking speed
		 */
		else
		{
			this->TargetVelocity = this->NormalWalkingSpeedWithWeapon;
		}
	}
	// Otherwise set the target velocity to be equal to the normal walking speed (without a weapon)
	else
	{
		this->TargetVelocity = this->NormalWalkingSpeed;
	}

	// Declaring a reference to the world timer manager 
	FTimerManager &TimerManager = this->GetWorldTimerManager();
	// Check if the timer that handles to update player character velocity is not active
	if (!TimerManager.IsTimerActive(this->TimerHandle_UpdateCharacterVelocity))
	{
		// It's not active then set the timer
		TimerManager.SetTimer(this->TimerHandle_UpdateCharacterVelocity, this,
			&AAlexCharacter::UpdateCharacterVelocity, 0.001, true);
	}
}

/*
  Called to start attacking once the player hit once/ holds down the attack button/ key
  this method activates single attack func 
 */
void AAlexCharacter::StartAttack()
{ // O(1)
	/*
	  Check if the player is changing a weapon, or if he has no weapon in hands,
	  or if he is near a wall, then return (cannot attack)
	 */
	if (this->bIsChangingWeapon || this->CurrentWeapon == nullptr || this->bIsNearWall) return;
	
	/*
	  Check if the player has a dual weapons, then check the left hand weapon
	  only in case the player is currently not using a gamepad (as left mouse button
	  is for attacking with a single weapon, or *attack with left hand weapon*)
	 */
	if (this->LeftHandCurrentWeapon != nullptr && this->bCanLeftHandAttack && !this->bIsLeftHandReloading
		&& this->LeftHandCurrentWeapon->PrepareAttack() && !this->bIsGamepadKey)
	{
		// Check if left hand weapon is a gun
		if (Cast<AGun>(this->LeftHandCurrentWeapon) != nullptr)
		{
			this->LeftHandWeaponAttack(false);
			/*
			  This left mouse click attack method! in case of a dual weapons, right hand should not attack by this method!
			  instead, the ADS (or right mouse click) should fire the right hand weapon!
			*/
			return;
		}
	}
	/*
	  Check main hand (right) in case not dual weapons
	  this code block is similar to the one above but is only for the right hand weapon instead
	 */
	if (this->bCanRightHandAttack && !this->IsReloading && this->CurrentWeapon->PrepareAttack())
	{
		this->RightHandWeaponAttack(false);
	}
	// Else means if the weapon is a melee weapon
	else
	{
		//..
	}
}

// Called for a single attack
void AAlexCharacter::SingleAttack() const
{ // O(1)
	if (this->bIsNearWall || this->CurrentWeapon == nullptr || this->bIsChangingWeapon) return;

	/*
	  Check if the left hand weapon is not null pointer and if the
	  player is not using a gamepad (means this is left mouse button attack
	  with dual weapons using the left hand weapon)
	 */
	if (this->LeftHandCurrentWeapon != nullptr && !this->bIsGamepadKey
		&& this->bCanLeftHandAttack && !this->bIsLeftHandReloading)
	{
		this->LeftHandCurrentWeapon->Attack();
	}
	// Otherwise that means the player is attacking with a single weapon
	else if (this->bCanRightHandAttack && !this->IsReloading)
	{
		this->CurrentWeapon->Attack();
	}
}

// Called when the player release attack input key
void AAlexCharacter::StopAttack()
{ // O(1)
	// Check the left hand weapon is not null and the player is not using a gamepad
	if (this->LeftHandCurrentWeapon != nullptr && !this->bIsGamepadKey)
	{
		this->StopLeftHandAttack();
	}
	// Otherwise means the player is using a single weapon
	else if (this->CurrentWeapon != nullptr)
	{
		this->StopRightHandAttack();
	}
}

/*
  Called to start attacking once the player hit once/ holds down the attack button
  (gamepad left trigger)/ (mouse right button).
  when the player has a dual weapons it activates the right hand weapon attack (mouse)
  or left hand weapon attack (gamepad)
  this method activated ads single attack
 */
void AAlexCharacter::AdsStartAttack()
{ // O(1)
	if (this->bIsNearWall || this->bIsChangingWeapon || this->CurrentWeapon == nullptr) return;
	
	// If there's a dual weapons, continue
	if (this->CurrentWeapon->IsDualWeapons)
	{
		/*
		  If the player is attacking using the dual weapons and not using a gamepad
		  means he attacked with ads button (mouse right button)
		 */
		if (!this->bIsGamepadKey)
		{
			// Check right hand weapon
			if (!this->IsReloading && this->bCanRightHandAttack && this->CurrentWeapon->PrepareAttack())
			{
				this->RightHandWeaponAttack(true);
			}
		}
		/*
		  attack using the left hand weapon while the player is using
		  a gamepad and has a dual weapons
		  attack using gamepad left trigger (where the player ads using a single gun)
		 */
		else if (!this->bIsLeftHandReloading && this->bCanLeftHandAttack && this->LeftHandCurrentWeapon->PrepareAttack())
		{
			this->LeftHandWeaponAttack(true);
		}
	}
	// Else means the player hold down the button to do ads
	else if (Cast<AGun>(this->CurrentWeapon) != nullptr && !this->CurrentWeapon->IsDualWeapons
		&& this->bCanAds)
	{
		// If weapon is reloading then cannot ads
		if (this->IsReloading)
		{
			return;
		}
		/*
		  If the weapon is not a sniper then set is aiming to true and adjust the speed
		  the sniper scope ads has been implemented in the blueprint child class of this
		 */
		if (this->CurrentWeapon->GetWeaponType() != Weapon_Sniper)
		{
			this->bIsAiming = true;
			this->AdjustSpeedWhileAds();
		}
		// The player cannot sprint (run) during ads 
		this->bCanCharacterRun = false;
	}
}

/*
  Called for a single attack using right hand weapon (mouse)
  or a single attack using left hand weapon (gamepad) 
  when the player has a dual weapons
*/
void AAlexCharacter::AdsSingleAttack() const
{ // O(1)
	if (this->bIsNearWall || this->CurrentWeapon == nullptr || this->LeftHandCurrentWeapon == nullptr
		|| this->bIsChangingWeapon) return;
	
	if (this->bIsGamepadKey && !this->bIsLeftHandReloading && this->bCanLeftHandAttack)
	{
		this->LeftHandCurrentWeapon->Attack();
	}
	else if (!this->IsReloading && this->bCanRightHandAttack)
	{
		this->CurrentWeapon->Attack();
	}
}

// Called for right hand weapon attack
void AAlexCharacter::RightHandWeaponAttack(const bool IsAdsAttack)
{ // O(1)
	/*
	  Check if the weapon type is gun and can keep shooting while holding the attack input key
	  In other words if it's an assault rifle, or a sub machine gun
	  in case true, method attack will keep executing till the player release the attack input key
	  (or when no ammo is left).
	 */
	if (this->CurrentWeapon->IsAutomaticWeapon)
	{
		this->CanReload = false;
		if (IsAdsAttack)
		{
			this->GetWorldTimerManager().SetTimer(this->TimerHandle_ReFire, this,
				&AAlexCharacter::AdsSingleAttack, this->CurrentWeapon->GetDelayBetweenAttacks(), true);
		}
		else
		{
			this->GetWorldTimerManager().SetTimer(this->TimerHandle_ReFire, this,
				&AAlexCharacter::SingleAttack, this->CurrentWeapon->GetDelayBetweenAttacks(), true);
		}
		this->IsAttacking = true;
	}
	// Otherwise means the gun is not automatic, then attack only once (pull trigger once and stop)
	else
	{
		// Setting a timer to reset can right hand attack to true
		this->GetWorldTimerManager().SetTimer(this->TimerHandle_ReFire, this,
				&AAlexCharacter::ResetCanRightHandAttack, this->CurrentWeapon->GetDelayBetweenAttacks(), false);
		// Right hand weapon cannot be reloaded during fire
		this->CanReload = false;
		// Right hand weapon cannot attack until the fire rate time for a second bullet fire ends
		this->bCanRightHandAttack = false;
		// Calling the attack method in the gun class for right hand weapon attack
		this->CurrentWeapon->Attack();
		// Setting is right hand weapon attacking to true
		this->IsAttacking = true;
	}
	// Called for activating gun recoil animation
	this->OnStartFireDel.Broadcast();
}

// Called for left hand weapon attack
void AAlexCharacter::LeftHandWeaponAttack(const bool IsAdsAttack)
{ // O(1)
	/*
	  Check if the weapon type is gun and can keep shooting while holding the attack input key
	  In other words if it's an assault rifle, or a sub machine gun
	  in case true, method attack will keep executing till the player release the attack input key
	  (or when no ammo is left).
	 */
	if (this->LeftHandCurrentWeapon->IsAutomaticWeapon)
	{
		this->LeftHandCanReload = false;
		if (IsAdsAttack)
		{
			this->GetWorldTimerManager().SetTimer(this->TimerHandle_LeftReFire, this,
				&AAlexCharacter::AdsSingleAttack, this->LeftHandCurrentWeapon->GetDelayBetweenAttacks(), true);
		}
		else
		{
			this->GetWorldTimerManager().SetTimer(this->TimerHandle_LeftReFire, this,
				&AAlexCharacter::SingleAttack, this->LeftHandCurrentWeapon->GetDelayBetweenAttacks(), true);
		}
		this->bIsLeftHandAttacking = true;
	}
	// Otherwise means the gun is not automatic, then attack only once (pull trigger once and stop)
	else
	{
		// Setting a timer to reset can left hand attack to true
		this->GetWorldTimerManager().SetTimer(this->TimerHandle_LeftReFire, this,
				&AAlexCharacter::ResetCanLeftHandAttack, this->LeftHandCurrentWeapon->GetDelayBetweenAttacks(), false);
		// Left hand weapon cannot be reloaded during fire
		this->LeftHandCanReload = false;
		// Left hand weapon cannot attack until the fire rate time for a second bullet fire ends
		this->bCanLeftHandAttack = false;
		// Calling the attack method in the gun class for right hand weapon attack
		this->LeftHandCurrentWeapon->Attack();
		// Setting is left hand weapon attacking to true
		this->bIsLeftHandAttacking = true;
	}
	this->OnStartFireDel.Broadcast();
}

// Called to stop attacking with the right hand weapon
void AAlexCharacter::StopRightHandAttack()
{ // O(1)
	// Called to stop attack with a right hand weapon
	this->CurrentWeapon->StopAttack();
	// Setting is the right hand weapon attacking to false
	this->IsAttacking = false;
	// Setting can reload to true
	this->CanReload = true;
	// Check if the weapon is a gun
	if (const AGun* CurrentGun = Cast<AGun>(this->CurrentWeapon))
	{
		// Called to stop recoil animation
		this->OnStopFireDel.Broadcast();
		// Check if the right hand gun needs a reload 
		if (CurrentGun->GetCurrentAmmo() == 0 && CurrentGun->GetCurrentTotalAmmo() > 0)
		{
			// Called to reload the right hand gun
			this->Reload();
		}
	}
}

// Called to stop attacking with the left hand weapon
void AAlexCharacter::StopLeftHandAttack()
{ // O(1)
	// Called to stop attack with a left hand weapon
	this->LeftHandCurrentWeapon->StopAttackLeft();
	// Setting is the left hand weapon attacking to false 
	this->bIsLeftHandAttacking = false;
	// Setting can left hand reload to true
	this->LeftHandCanReload = true;
	// Check if the left hand weapon is a gun
	if (const AGun* LeftHandCurrentGun = Cast<AGun>(this->LeftHandCurrentWeapon))
	{
		this->OnStopFireDel.Broadcast();
		if (LeftHandCurrentGun->GetCurrentAmmo() == 0 && LeftHandCurrentGun->GetCurrentTotalAmmo() > 0)
		{
			this->Reload();
		}
	}
}

/*
  Called to stop attacking once the player release the when having dual weapons
  if using a mouse then the weapon in the right hand will stop attacking
  if using a gamepad then the weapon in the left hand will stop attacking
 */
void AAlexCharacter::AdsStopAttack()
{ // O(1)
	// if there's a dual guns, continue
	if (this->LeftHandCurrentWeapon != nullptr)
	{
		/*
		  Check if the right hand weapon is not a null pointer
		  and the player is not using a gamepad, means
		  that the player stopped attacking using dual weapons
		  by releasing the button where it attacks with the right hand weapon
		 */
		if (this->CurrentWeapon != nullptr && !this->bIsGamepadKey)
		{
			// Call attack method using right hand weapon
			this->StopRightHandAttack();
		}
		// Otherwise check if the player is using a gamepad
		else if (this->bIsGamepadKey)
		{ /*
			The player released the left trigger button while having dual weapons
			 means stopped attacking with the left hand weapon (where the player do ads when having a single gun)
		   */
			// Call attack method using left hand weapon
			this->StopLeftHandAttack();
		}
	}
	// Else check if the player is aiming with a weapon to stop aiming
	else if (this->bIsAiming)
	{
		this->bCanCharacterRun = true;
		this->AdjustSpeedAfterReleaseAds();
	}
}


/* Reset can attack functions */

// Called to reset whether the right hand can attack or cannot
void AAlexCharacter::ResetCanRightHandAttack()
{ // O(1)
	this->bCanRightHandAttack = true;
	this->CanReload = true;

	AGun* CurrentGun = Cast<AGun>(this->CurrentWeapon);
	if (CurrentGun != nullptr && CurrentGun->GetCurrentAmmo() == 0)
	{
		this->Reload();
	}
}

// Called to reset whether the left hand can attack or cannot
void AAlexCharacter::ResetCanLeftHandAttack()
{ // O(1)
	this->bCanLeftHandAttack = true;
	this->LeftHandCanReload = true;

	AGun* LeftHandCurrentGun = Cast<AGun>(this->LeftHandCurrentWeapon);
	if (LeftHandCurrentGun != nullptr && LeftHandCurrentGun->GetCurrentAmmo() == 0)
	{
		this->Reload();
	}
}

// Called when the player press reload input key
void AAlexCharacter::Reload()
{ // O(1)
	// If the player is changing between weapons he can't reload a gun, return
	if (this->bIsChangingWeapon) return;
	
	// Check left hand gun
	if (this->LeftHandCurrentWeapon != nullptr && this->LeftHandCanReload
		&& !(this->bIsLeftHandReloading || this->bIsLeftHandAttacking))
	{
		AGun* LeftHandCurrentGun = Cast<AGun>(this->LeftHandCurrentWeapon);
		if (LeftHandCurrentWeapon != nullptr)
		{
			/*
			  Check if the mag of the left hand gun isn't full
			  & there's an ammo of this gun's ammo type available for a reload
			 */
			if (!(LeftHandCurrentGun->GetIsMagFull() || LeftHandCurrentGun->GetAmmoAmountAvailableForReload() <= 0))
			{
				this->bCanLeftHandAttack = false;
				this->bIsLeftHandReloading = true;
				// Called to report for a reloading noise to the AI pawns
				this->ReportNoise(this->LeftHandCurrentWeapon->GetActorLocation(), 0.7f);
			}
		}
	}
	// Check right hand gun
	if (this->CurrentWeapon != nullptr && this->CanReload && !(this->IsReloading || this->IsAttacking))
	{
		AGun* CurrentGun = Cast<AGun>(this->CurrentWeapon);
		if (CurrentGun != nullptr)
		{
			/*
			  Check if the mag of the right hand gun isn't full
			  & there's an ammo of this gun's ammo type available for a reload
			 */
			if (!(CurrentGun->GetIsMagFull() || CurrentGun->GetAmmoAmountAvailableForReload() <= 0))
			{
				// Release aiming if the player is
				if (this->bIsAiming)
				{
					this->bIsAiming = false;
					this->OnForceReleaseAds.Broadcast();
				}
				// Setting can ads to false if it's not and the player has a single weapon (not dual)
				if (this->bCanAds && !this->CurrentWeapon->IsDualWeapons)
				{
					this->bCanAds = false;
				}
				this->bCanRightHandAttack = false;
				this->IsReloading = true;
				// Called to report for a reloading noise to the AI pawns
				this->ReportNoise(this->CurrentWeapon->GetActorLocation(), 0.7f);
			}
		}
	}
}


/*******************************************************/


/* Player interaction with the interact-able actors in the world functions */

/*
  Called when the player hit once the button/ key to interact
  with an interactable actors across the world
 */
void AAlexCharacter::Interact()
{
	// If the player cannot interact, return
	if (!this->CanInteract) return;
	// Start location of the line trace
	FVector StartLocation = this->PlayerCamera->GetComponentLocation();
	// End location of the line trace
	FRotator Rotation = this->PlayerCamera->GetComponentRotation();
	FVector EndLocation = StartLocation + Rotation.Vector() * this->PlayerInteractDistance;
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	// Firing the line trace for an objects types of collision channel interactable
	bool Result = GetWorld()->LineTraceSingleByChannel(OUT
		HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_GameTraceChannel1, Params);
	// If there's no hit, or no actor was hit, return
	if (!Result || HitResult.GetActor() == nullptr) return;
	// Trying to cast the actor was hit to an interactable actor
	AInteractable* InteractableActor = Cast<AInteractable>(HitResult.GetActor());
	if (InteractableActor == nullptr) return;
	// Check the type of interactable actor
	this->CanInteract = false;
	switch(InteractableActor->InteractableType)
	{
		case Interactable_Door:
			{ // The player interacted with a door
				this->InteractDoor(Cast<ARegularDoor>(InteractableActor));
				break;
			}
		case Interactable_Item:
			{ // The player interacted with an item
				this->ItemToPickUp = Cast<AItem>(InteractableActor);
				this->InteractItem();
				break;
			}
		case Interactable_MysteryBox:
			{ // The player interacted with a mystery box
				this->InteractMysteryBox(Cast<AMysteryBoxBase>(InteractableActor));
				break;
			}
		case Interactable_WallWeapon:
			{ // The player interacted with a wall weapon buy
				this->InteractWallWeapon(Cast<AWallWeaponBuy>(InteractableActor));
				break;
			}
		case Interactable_PowerStone:
			{ // The player interacted with a power stone
				this->InteractPowerStone(Cast<APowerStone>(InteractableActor));
				break;
			}
		case Interactable_Cash:
			{ // The player interacted with a cash actor
				this->InteractCashActor(Cast<ACashActor>(InteractableActor));
				break;
			}
		case Interactable_Obstacle:
			{ // The player interacted with an obstacle
				this->InteractObstacle(Cast<AObstacle>(InteractableActor));
				break;
			}
		case Interactable_PartsBuildingTable:
			{ // The player interacted with a parts building table
				this->InteractQuestItemsCraftingTable(Cast<AQuestItemsCraftingTable>(InteractableActor));
			}
		default: ;
	}
	// The player can interact only after the first interaction is done.
	this->CanInteract = true;
}

// Called when the player release the interact button/ key
void AAlexCharacter::ReleaseInteract()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AQuestItemsCraftingTable::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		if (Actor != nullptr)
		{
			if (AQuestItemsCraftingTable* QuestItemsCraftingTable = Cast<AQuestItemsCraftingTable>(Actor))
			{
				if (QuestItemsCraftingTable->GetIsInUse())
				{
					QuestItemsCraftingTable->ReleaseUsingTable();
				}
			}
		}
	}
}

// Called when the player holds down the button/ key to interact with an item actor in the world
void AAlexCharacter::InteractItem()
{
	if (this->ItemToPickUp == nullptr || this->Inventory == nullptr) return;
	
	// Check the Item type
	switch (this->ItemToPickUp->ItemType)
	{
		case Item_Ammo: { this->InteractAmmo(); break; }
		case Item_Weapon: { this->InteractWeapon(); break; }
		case Item_Quest: { this->InteractQuest(Cast<AQuestItem>(this->ItemToPickUp)); break; }
		default: ;
	}
	this->ItemToPickUp = nullptr;
}

// Called when the player holds down the button/ key to interact with a door actor in the world
void AAlexCharacter::InteractDoor(ARegularDoor* InDoor)
{
	if (InDoor == nullptr) return;
	// Check if the door needs a key to open
	if (ADoorKeyRequired* DoorKeyReq = Cast<ADoorKeyRequired>(InDoor))
	{
		//..
	}
	// Otherwise it's just a regular door
	else
	{
		const int32 DoorCost = InDoor->GetCost();
		// Check the cost of the door
		if (DoorCost > 0)
		{
			// If the door requires payment but it's already opened, return
			if (InDoor->IsDoorOpen()) return;
			// Check if the player has enough points to pay to open the door
			if (this->CurrentPoints >= InDoor->GetCost())
			{
				// Called to open the door
				InDoor->ToggleDoor(Cast<AActor>(this));
				// If the door is connected to another door (like double doors)
				if (InDoor->GetSecondDoor() != nullptr)
				{
					// Called to interact with the second door
					this->InteractDoor(InDoor->GetSecondDoor());
				}
				// Check if the door widget should be removed after opened
				if (InDoor->GetHideWidgetAfterOpened())
				{
					// Hiding the door's interaction widget
					InDoor->SetShowWidget(false);
				}
				// Called to decrease the player points by the door's cost 
				this->DecreasePlayerPoints(DoorCost, true);
				// Check if opening the door should spawn an enemy spawner
				if (InDoor->IsEnemySpawnerAfterOpenedAllowed() && !InDoor->bSpawnedEnemySpawner
					&& InDoor->GetDoorEnemySpawnerArrowComponent() != nullptr
					&& IsValid(InDoor->GetEnemySpawnerClass()) && this->MainGameMode != nullptr)
				{
					// Spawning a new enemy spawner defined in the door actor arrow component
					InDoor->bSpawnedEnemySpawner = this->MainGameMode->SpawnEnemySpawner(
						InDoor->GetDoorEnemySpawnerArrowComponent()->GetComponentLocation(),
						InDoor->GetEnemySpawnerClass());
				}
			}
		}
		// Otherwise open the door for free (cost 0)
		else
		{
			// Called to open the door
			InDoor->ToggleDoor(Cast<AActor>(this));
			// If the door is connected to another door (like double doors)
			if (InDoor->GetSecondDoor() != nullptr)
			{
				// Called to interact with the second door
				this->InteractDoor(InDoor->GetSecondDoor());
			}
			// Check if the door widget should be removed after opened
			if (InDoor->GetHideWidgetAfterOpened())
			{
				// Hide the door's interaction widget
				InDoor->SetShowWidget(false);
			}
		}
	}
}

// Called when the player holds down the button/ key to interact with a weapon actor in the world
void AAlexCharacter::InteractWeapon()
{
	if (this->WeaponsWheel == nullptr || this->Inventory == nullptr)
	{
		this->ItemToPickUp = nullptr;
		return;
	}
	// Check if the weapon does already exists
	if (this->CurrentWeapon != nullptr && this->WeaponsWheel->CheckIfWeaponExists(this->ItemToPickUp->GetItemObject()))
	{
		// If the weapon is from a mystery box
		if (this->CurrentMysteryBoxCost > 0)
		{
			// Giving back half cost as points to the player 
			this->IncreasePlayerPoints(this->CurrentMysteryBoxCost / 2, false);
		}
		this->CurrentMysteryBoxCost = 0;
		this->ItemToPickUp = nullptr;
		return;
	}
	// Check if the weapon item successfully been added to the inventory 
	if (this->Inventory->TryAddItem(this->ItemToPickUp->GetItemObject(), true))
	{
		// Called after succeeded to add a weapon item
		this->SuccessInteractWeapon();
	}
	/*
	  Otherwise, try to [test] remove the item in the player's hands (if any),
	  and then [test] add the weapon & if succeeded,
	  then really remove the current item and add the new one 
	 */
	else if (this->CurrentItemObjectInUse != nullptr && this->Inventory->TestRemoveItemCheckRoomForNewItem(
		this->CurrentItemObjectInUse,this->ItemToPickUp->GetItemObject()))
	{
		// Removing the current item in the player's hands 
		this->Inventory->RemoveItem(this->CurrentItemObjectInUse, false, false);
		// Adding the new weapon item to inventory and to the weapons selections wheel
		if (this->Inventory->TryAddItem(this->ItemToPickUp->GetItemObject(), true))
		{
			// Called after succeeded to add a weapon item
			this->SuccessInteractWeapon();
		}
	}
	/*
	  Otherwise means the player cannot take the weapon,
	  then check if it's coming from a wall weapon state to destroy the spawned weapon
	 */
	else if (this->IsWallWeaponState)
	{
		this->ItemToPickUp->Destroy();
		this->ItemToPickUp = nullptr;
	}
}

// Called if interact weapon succeeded to find space for a new weapon
void AAlexCharacter::SuccessInteractWeapon()
{
	// Check pick up sound to play
	if (this->ItemToPickUp->PickupSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this->GetWorld(),
				this->ItemToPickUp->PickupSound, this->GetActorLocation());
		// Called to report for a noise to the AI
		this->ReportNoise(this->GetActorLocation(), 0.9f);
	}
	// Check if a wall weapon
	if (this->IsWallWeaponState)
	{
		// Called to decrease the player points as the purchase succeeded
		this->DecreasePlayerPoints(this->CurrentWallWeaponCost, true);
		// Destroying the spawned weapon of the wall weapon buy
		this->ItemToPickUp->Destroy();
		this->IsWallWeaponState = false;
		this->CurrentWallWeaponCost = 0;
		return;
	}
	if (this->CurrentMysteryBoxCost > 0)
	{
		AWeapon* Weapon = Cast<AWeapon>(this->ItemToPickUp);
		if (Weapon != nullptr && Weapon->MysteryBoxRef != nullptr)
		{
			Weapon->MysteryBoxRef->ImmediateClose();
		}
		this->CurrentMysteryBoxCost = 0;
	}
	// const FName ItemId = this->ItemToPickUp->GetItemId();
	// this->MyGameInstance->DestroyItem(ItemId);
	this->ItemToPickUp->Destroy();
}

// Called when the player holds down the button/ key to interact with a mystery box actor in the world
void AAlexCharacter::InteractMysteryBox(AMysteryBoxBase* InMysteryBox)
{
	// If the mystery is a null or it is open, or if it is not active, return
	if (InMysteryBox == nullptr || InMysteryBox->IsMysteryBoxOpen()
		|| !InMysteryBox->IsMysteryBoxActive()) return;
	
	const int32 BoxCost = InMysteryBox->GetCost();
	this->CurrentMysteryBoxCost = BoxCost;
	// If the player has enough points to open the mystery box
	if (this->CurrentPoints >= BoxCost)
	{
		// Called to decrease the player points
		this->DecreasePlayerPoints(BoxCost, true);
		// Called to open the mystery box
		InMysteryBox->OpenBox();
	}
}

// Called when the player holds down the button/ key to interact with an ammo item actor in the world
void AAlexCharacter::InteractAmmo()
{
	// Check if the ammo item object is valid to continue 
	UAmmoItemObject* CurrentAmmoItemObject = Cast<UAmmoItemObject>(this->ItemToPickUp->GetItemObject());
	if (CurrentAmmoItemObject == nullptr)
	{
		this->ItemToPickUp = nullptr;
		return;
	}

	// Calculate the ammo amount lack of this ammo type in the inventory
	int32 AmountLackInv = CurrentAmmoItemObject->MaxAmmoForType -
		this->Inventory->GetAmmoByWeaponType(CurrentAmmoItemObject->AmmoTypeByWeapon);

	// If the amount lack is <= 0, then add a notifying widget and return
	if (AmountLackInv <= 0)
	{
		if (this->NoMoreAmmoOfTypeSfx != nullptr)
		{
			this->OnNoMoreAmmoAllowedOfType.Broadcast(CurrentAmmoItemObject->AmmoTypeByWeapon);
			UGameplayStatics::PlaySound2D(this, this->NoMoreAmmoOfTypeSfx);
		}
		this->ItemToPickUp = nullptr;
		return;
	}
	
	// Getting the ammo item objects from the player's inventory
	TArray<UAmmoItemObject*> AmmoItemObjects = this->Inventory->GetAmmoItemObjectsOfType(
		CurrentAmmoItemObject->AmmoTypeByWeapon);

	bool bAdded = false;

	// Adding ammo amounts to the already equipped ammo items of same type in the inventory
	for (UAmmoItemObject* AmmoItemObject : AmmoItemObjects)
	{
		if (AmmoItemObject != nullptr && AmmoItemObject->AmmoTypeByWeapon == CurrentAmmoItemObject->AmmoTypeByWeapon)
		{
			const int32 FinalAmountToAdd = FMath::Min(AmmoItemObject->MaxAmmoAmountPerItem-AmmoItemObject->CurrentTotalAmmo,
				CurrentAmmoItemObject->AmmoAmountToAdd);
			if (FinalAmountToAdd > 0)
			{
				CurrentAmmoItemObject->AmmoAmountToAdd -= FinalAmountToAdd;
				AmmoItemObject->CurrentTotalAmmo += FinalAmountToAdd;
				AmmoItemObject->AmmoAmountToAdd = AmmoItemObject->CurrentTotalAmmo;
				AmountLackInv -= FinalAmountToAdd;
				if (!bAdded) bAdded = true;
			}
		}
	}
	/*
	  Assigning the amount left to add as the minimum between the
	  ammo amount to add from the ammo item object, the amount lack in inventory of the ammo type,
	  and the max ammo amount per item (in inventory).
	  max iterator is maximum ammo items of type allowed.
	 */
	int32 AmountLeftToAdd = FMath::Min(CurrentAmmoItemObject->AmmoAmountToAdd,
		FMath::Min(AmountLackInv, CurrentAmmoItemObject->MaxAmmoAmountPerItem));
	int8 MaxIterator = CurrentAmmoItemObject->MaxAmmoForType / CurrentAmmoItemObject->MaxAmmoAmountPerItem;
	const int8 Iterators = MaxIterator;
	int32 CurrentObjectAmmoAmountToAdd = CurrentAmmoItemObject->AmmoAmountToAdd;
	int8 AmmoItemsCountOfSameType = AmmoItemObjects.Num();

	// Looping to add ammo items to inventory as long as all conditions below are true
	while (CurrentObjectAmmoAmountToAdd > 0 && AmountLeftToAdd > 0 && AmmoItemsCountOfSameType < Iterators && MaxIterator > 0)
	{
		/*
		  If already added the 1st ammo item object, then update
		  the ammo item object by creating new object copied of the
		  last added ammo item object (to prevent pointer issues)
		 */
		if (Iterators != MaxIterator)
		{
			// Creating a new ammo item object
			UAmmoItemObject* AmmoItemObject = NewObject<UAmmoItemObject>();
			// Copying all properties from old ammo item object to the new one
			CurrentAmmoItemObject->Copy(AmmoItemObject);
			// Reassign pointer of current ammo item object to the new one
			CurrentAmmoItemObject = AmmoItemObject;
		}
		// If the ammo item object succeeded to be added to inventory
		if (this->Inventory->TryAddItem(CurrentAmmoItemObject, true))
		{
			CurrentAmmoItemObject->CurrentTotalAmmo = AmountLeftToAdd;
			CurrentAmmoItemObject->AmmoAmountToAdd -= AmountLeftToAdd;
			// Reassign amount left to add again
			AmountLeftToAdd = FMath::Min(CurrentAmmoItemObject->AmmoAmountToAdd,
				FMath::Min(AmountLackInv, CurrentAmmoItemObject->MaxAmmoAmountPerItem));
			++AmmoItemsCountOfSameType;
			if (!bAdded) bAdded = true;
		}
		CurrentObjectAmmoAmountToAdd = CurrentAmmoItemObject->AmmoAmountToAdd;
		--MaxIterator;
	}
	// If ammo amount added after all, then play sound effect
	if (bAdded && this->ItemToPickUp->PickupSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this->GetWorld(),
				this->ItemToPickUp->PickupSound, this->GetActorLocation());
		// Called to report for a noise to AI
		this->ReportNoise(this->GetActorLocation(), 0.9f);
	}
	if (CurrentAmmoItemObject->AmmoAmountToAdd == 0)
	{
		// const FName ItemId = this->ItemToPickUp->GetItemId();
		// MyGameInstance->DestroyItem(ItemId);
		this->ItemToPickUp->Destroy();
	}
	// Updating ammo amount in the player's inventory
	this->Inventory->UpdateAmmoAmounts();
	/*
	  Trying to re initialize the player's gun in case he has a gun and the gun's ammo
	  type is the same type as the ammo item the player has picked up
	 */
	if (this->CurrentWeapon != nullptr)
	{
		AGun* Gun = Cast<AGun>(this->CurrentWeapon);
		if (Gun != nullptr && CurrentAmmoItemObject->AmmoTypeByWeapon == Gun->GetWeaponType())
		{
			Gun->InitializeAmmo();
			AGun* LeftHandGun = Cast<AGun>(this->LeftHandCurrentWeapon);
			if (LeftHandGun != nullptr)
			{
				LeftHandGun->InitializeAmmo();
			}
		}
	}
	// Check if the ammo item came from a purchase of wall weapon buy (same weapon with the player)
	if (this->IsWallWeaponState)
	{
		/*
	      If added ammo amounts to inventory, means the player got
	      an ammo of a wall weapon buy purchase, then decrease his points
	      with the right current wall weapon but cost
		 */
		if (bAdded)
		{
			// Called to decrease the player's points
			this->DecreasePlayerPoints(this->CurrentWallWeaponCost, true);
		}
		this->CurrentWallWeaponCost = 0;
		this->IsWallWeaponState = false;
		this->ItemToPickUp->Destroy();
	}
	this->ItemToPickUp = nullptr;
}

// Called when the player holds down the button/ key to interact with a wall weapon buy actor in the world
void AAlexCharacter::InteractWallWeapon(AWallWeaponBuy* InWallWeaponBuy)
{
	/*
	  If the player was reloading then he can't purchase a wall weapon
	  (to prevent free ammo glitch)
	 */
	if (InWallWeaponBuy == nullptr || this->IsReloading || this->bIsLeftHandReloading) return;

	// Check if the player has enough points to buy
	const int32 WallWeaponCost = InWallWeaponBuy->GetCost();
	if (WallWeaponCost > this->CurrentPoints) return;
	/*
	  If the wall weapon buy is buying a weapon
	  means the player has no weapon in hands or the weapon
	  is different than the one in the current wall weapon
	 */
	if (WallWeaponCost == InWallWeaponBuy->CostCopy)
	{
		AWeapon* SpawnedWeapon = Cast<AWeapon>(this->GetWorld()->SpawnActor(InWallWeaponBuy->GetWeaponClass()));
		if (SpawnedWeapon == nullptr) return;
		SpawnedWeapon->SetActorHiddenInGame(true);
		SpawnedWeapon->SetActorEnableCollision(ECollisionEnabled::NoCollision);
		this->CurrentWallWeaponCost = WallWeaponCost;
		this->IsWallWeaponState = true;
		this->ItemToPickUp = SpawnedWeapon;
		this->InteractItem();
	}
	/*
	  Else check if the player has a gun, if he has that means
	  the gun is the same as the gun in the current wall, then try to
	  spawn ammo item to increase ammo with the same ammo type
	  of the current gun in his hands
	 */
	else if (const AGun* CurrentGun = Cast<AGun>(this->CurrentWeapon))
	{
		// Ammo amount lack of type in the inventory (total)
		const int32 AmmoLack = CurrentGun->MaxAmmoForGun - CurrentGun->GetCurrentTotalAmmo();
		/*
		 Check if ammo lack is > 0 to continue & that we have a valid reference
		 to the right ammo item class to spawn
		 */
		if (AmmoLack <= 0 || !this->AmmoItemsClasses.Contains(CurrentGun->GetWeaponType())) return;
		const TSubclassOf<AAmmoItem> AmmoItemClass = this->AmmoItemsClasses[CurrentGun->GetWeaponType()];
		// Validating ammo item class reference
		if (!IsValid(AmmoItemClass)) return;
		// Spawning ammo item with always spawn checked
		AAmmoItem* AmmoItemToSpawn = Cast<AAmmoItem>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
			this, AmmoItemClass, FTransform(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
		UGameplayStatics::FinishSpawningActor(AmmoItemToSpawn, FTransform());
		
		// Validating the new spawned ammo item
		if (AmmoItemToSpawn == nullptr) return;
		
		// Setting correct values before calling method to add ammo item
		AmmoItemToSpawn->SetAmmoType(CurrentGun->GetWeaponType());
		AmmoItemToSpawn->SetAmmoItemAmmoAmount(AmmoLack);
		AmmoItemToSpawn->SetActorHiddenInGame(true);
		AmmoItemToSpawn->SetActorEnableCollision(ECollisionEnabled::NoCollision);
		this->IsWallWeaponState = true;
		this->ItemToPickUp = AmmoItemToSpawn;
		this->CurrentWallWeaponCost = InWallWeaponBuy->GetCost();
		// Called to force interact with the spawned ammo item
		this->InteractItem();
	}
}

// Called when the player holds down the button/ key to interact with a power stone actor in the world
void AAlexCharacter::InteractPowerStone(APowerStone* InPowerStone)
{
	// Validating the power stone
	if (InPowerStone == nullptr) return;
	// Check if the player has enough points to buy the power stone
	if (InPowerStone->GetCost() > this->CurrentPoints) return;

	// switch on the current power stone type to be bought
	switch(InPowerStone->GetPowerStoneType())
	{
		case PowerStone_DoubleSouls: {this->InteractDoubleSoulsPowerStone(InPowerStone); break;}
		case PowerStone_RapidFire: {this->InteractRapidFirePowerStone(InPowerStone); break;}
		case PowerStone_BigShoulders: {this->InteractBigShouldersPowerStone(InPowerStone); break;}
		case PowerStone_Speedy: {this->InteractSpeedyPowerStone(InPowerStone); break;}
		default: ;
	}
}

// Called when the player holds down the button/ key to interact with a double souls power stone in the world
void AAlexCharacter::InteractDoubleSoulsPowerStone(APowerStone* InPowerStone)
{
	if (this->PowerStonesByTypes.Contains(PowerStone_DoubleSouls)) return;
	
	this->DecreasePlayerPoints(InPowerStone->GetCost(), true);
	this->FullHealth = FMath::Min(this->FullHealth * 2, this->DoubleSoulsFullHealth);
	this->PowerStonesByTypes.AddUnique(PowerStone_DoubleSouls);
	this->PowerStones.AddUnique(InPowerStone);
	this->CurrentHealth = this->FullHealth;
	this->OnPlayerCurrentHealthChanged.Broadcast();
	this->OnEquipPowerStoneDel.Broadcast();
}

// Called when the player holds down the button/ key to interact with a rapid fire power stone in the world
void AAlexCharacter::InteractRapidFirePowerStone(APowerStone* InPowerStone)
{
	if (this->PowerStonesByTypes.Contains(PowerStone_RapidFire)) return;
	
	this->DecreasePlayerPoints(InPowerStone->GetCost(), true);
	this->PowerStonesByTypes.AddUnique(PowerStone_RapidFire);
	this->PowerStones.AddUnique(InPowerStone);
	if (Cast<AGun>(this->CurrentWeapon) != nullptr)
	{
		this->CurrentWeapon->SetDelayBetweenRapidAttacks();
	}
	if (Cast<AGun>(this->LeftHandCurrentWeapon) != nullptr)
	{
		this->LeftHandCurrentWeapon->SetDelayBetweenRapidAttacks();
	}
	this->OnEquipPowerStoneDel.Broadcast();
}

// Called when the player holds down the button/ key to interact with a big shoulders power stone in the world
void AAlexCharacter::InteractBigShouldersPowerStone(APowerStone* InPowerStone)
{
	if (this->PowerStonesByTypes.Contains(PowerStone_BigShoulders)) return;
	
	this->DecreasePlayerPoints(InPowerStone->GetCost(), true);
	this->PowerStonesByTypes.AddUnique(PowerStone_BigShoulders);
	this->PowerStones.AddUnique(InPowerStone);
	this->Inventory->Columns *= InPowerStone->GetPowerValue();
	this->Inventory->Rows *= InPowerStone->GetPowerValue();
	this->Inventory->OnRowsColumnsChangedDel.Broadcast();
	this->OnEquipPowerStoneDel.Broadcast();
}

// Called when the player holds down the button/ key to interact with a speedy power stone in the world
void AAlexCharacter::InteractSpeedyPowerStone(APowerStone* InPowerStone)
{
	if (this->PowerStonesByTypes.Contains(PowerStone_Speedy)) return;
	
	this->DecreasePlayerPoints(InPowerStone->GetCost(), true);
					
	this->MaximumWalkingSpeed *= this->SpeedyPowerStonePowerMultiplier;
	this->NormalWalkingSpeed *= this->SpeedyPowerStonePowerMultiplier;

	if (this->CurrentWeapon != nullptr)
	{
		this->MaximumWalkingSpeedWithWeapon *= this->SpeedyPowerStonePowerMultiplier;
		this->NormalWalkingSpeedWithWeapon *= this->SpeedyPowerStonePowerMultiplier;
	}
					
	this->PowerStonesByTypes.AddUnique(PowerStone_Speedy);
	this->PowerStones.AddUnique(InPowerStone);
	this->OnEquipPowerStoneDel.Broadcast();
}

void AAlexCharacter::InteractQuest(AQuestItem* InQuestItem)
{
	if (InQuestItem == nullptr || this->QuestInventory == nullptr) return;

	this->QuestInventory->AddQuestItem(InQuestItem->GetItemObject());

	if (InQuestItem->PickupSound != nullptr)
	{
		UGameplayStatics::PlaySound2D(this, InQuestItem->PickupSound, 1.0f,
			1.0f, 0.0f, nullptr,
			nullptr, false);
	}
	
	if (AQuestItemsCraftingTable* UsedTable = InQuestItem->GetUsedTable())
	{
		UsedTable->SetCanUseTable(true);
		if (UBoxComponent* TableBoxCollisionComp = UsedTable->GetBoxCollisionComponent())
		{
			TableBoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		if (InQuestItem->GetQuestType() == EQuestType::Shield)
		{
			AttachShieldToPlayer(InQuestItem->GetItemObject());
		}
	}
	
	InQuestItem->Destroy();
}

// Called when the player holds down the button/ key to interact with a quest items crafting table actor in the world
void AAlexCharacter::InteractQuestItemsCraftingTable(AQuestItemsCraftingTable* InQuestItemsCraftingTable) const
{
	if (InQuestItemsCraftingTable == nullptr || this->QuestHandlerComp == nullptr) return;

	switch (InQuestItemsCraftingTable->GetQuestItemsCraftingTableType())
	{
		case QuestItemsCraftingTable_Shield:
			{
				InQuestItemsCraftingTable->UseTable(this->QuestHandlerComp->HandleShieldQuest());
				break;
			}
		case QuestItemsCraftingTable_Keys:
			{
				InQuestItemsCraftingTable->UseTable(this->QuestHandlerComp->HandleKeyQuest());
				break;
			}
		default: ;
	}
}

// Called when the player holds down the button/ key to interact with a cash actor in the world
void AAlexCharacter::InteractCashActor(ACashActor* InCashActor)
{
	if (InCashActor == nullptr) return;
	
	this->IncreasePlayerPoints(InCashActor->GetPointsToGive(), true);
	
	InCashActor->Destroy();
}

// Called when the player holds down the button/ key to interact with an obstacle actor in the world
void AAlexCharacter::InteractObstacle(AObstacle* InObstacleActor)
{
	if (InObstacleActor == nullptr || InObstacleActor->GetIsRemoving()) return;

	if (this->CurrentPoints >= InObstacleActor->GetCost())
	{
		InObstacleActor->RemoveObstacle();
		this->DecreasePlayerPoints(InObstacleActor->GetCost(), true);
	}
}


/*
  Called to look for an interactable actor in a line trace
  of the player's character, to update the actors interaction widgets
  like showing them or hiding them from the viewport
 */
void AAlexCharacter::LookForInteractableActorInteractionWidget()
{
	// Line trace to interactable objects collision channel
	FVector StartLocation = this->PlayerCamera->GetComponentLocation();
	FRotator Rotation = this->PlayerCamera->GetComponentRotation();
	FVector EndLocation = StartLocation + (Rotation.Vector() * this->PlayerInteractDistance);
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// Firing the line trace
	bool Result = GetWorld()->LineTraceSingleByChannel(OUT
		HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_GameTraceChannel1, Params);
	/*
	  If there's no hit or no actor got hit by the line trace,
	  and there was an interactable actor that got hit by same type of line trace
	  before, then hide it's interaction widget
	 */
	if ((!Result || HitResult.GetActor() == nullptr) && this->ForwardInteractableActor != nullptr)
	{
		UUserWidget* Widget = this->ForwardInteractableActor->GetInteractionWidget();
		if (Widget != nullptr && Widget->IsInViewport())
		{
			// Remove interaction widget from the viewport (not functioning & non visible) 
			Widget->RemoveFromViewport();	
		}
	}
	// Otherwise check if there was a hit by the line trace
	else if (Result)
	{
		AInteractable* InteractableActor = Cast<AInteractable>(HitResult.GetActor());
		/*
		   If the previous actor was not null pointer and is not the same as
		   the new one, then remove the previous actor's interaction widget from the viewport 
		 */
		if (this->ForwardInteractableActor != nullptr && this->ForwardInteractableActor != InteractableActor)
		{
			if (UUserWidget* Widget = this->ForwardInteractableActor->GetInteractionWidget())
			{
				if (Widget->IsInViewport())
				{
					Widget->RemoveFromViewport();
				}
			}
		}
		// Updating the new interactable actor that got the hit (even if null pointer)
		this->ForwardInteractableActor = InteractableActor;
		// Check if the new interactable actor is not null pointer to continue
		if (this->ForwardInteractableActor == nullptr) return;
		UInteractionWidget* Widget = Cast<UInteractionWidget>(this->ForwardInteractableActor->GetInteractionWidget());
		// check if the interaction widget of the new interactable actor is not null pointer
		if (Widget == nullptr) return;
		switch (this->ForwardInteractableActor->InteractableType)
		{
			case Interactable_MysteryBox:
				{
					this->CheckMysteryBoxInteractionWidget(Cast<AMysteryBoxBase>(this->ForwardInteractableActor), Widget);
					break;
				}
			case Interactable_WallWeapon:
				{
					this->CheckWallWeaponBuyInteractionWidget(Cast<AWallWeaponBuy>(this->ForwardInteractableActor), Widget);
					break;
				}
			case Interactable_Item:
				{
					this->CheckAmmoItemInteractionWidget(Cast<AAmmoItem>(this->ForwardInteractableActor), Widget);
					this->CheckWeaponItemInteractionWidget(Cast<AWeapon>(this->ForwardInteractableActor), Widget);
					this->CheckQuestItemInteractionWidget(Cast<AQuestItem>(this->ForwardInteractableActor), Widget);
					break;
				}
			case Interactable_Door:
				{
					this->CheckDoorInteractionWidget(Cast<ARegularDoor>(this->ForwardInteractableActor), Widget);
					break;
				}
			case Interactable_PowerStone:
				{
					this->CheckPowerStoneInteractionWidget(Cast<APowerStone>(this->ForwardInteractableActor), Widget);
					break;
				}
			case Interactable_Obstacle:
				{
					this->CheckObstacleInteractionWidget(Cast<AObstacle>(this->ForwardInteractableActor), Widget);
					break;
				}
			case Interactable_PartsBuildingTable:
				{
					this->CheckQuestItemsCraftingTableInteractionWidget(Cast<AQuestItemsCraftingTable>(this->ForwardInteractableActor), Widget);
				}
			default: ;
		}
	}
}

// Called to update mystery box interaction widget if in sight of the player
void AAlexCharacter::CheckMysteryBoxInteractionWidget(AMysteryBoxBase* InMysteryBox, UInteractionWidget* Widget) const
{
	if (InMysteryBox == nullptr) return;
	
	Widget->SetActionTextColor(InMysteryBox->IsMysteryBoxActive());
	Widget->SetCostTextColor(InMysteryBox->GetCost() <= this->CurrentPoints);

	if (Widget->IsInViewport() && InMysteryBox->IsMysteryBoxOpen())
	{
		Widget->RemoveFromViewport();
		return;
	}

	if (!Widget->IsInViewport() && !InMysteryBox->IsMysteryBoxOpen())
	{
		Widget->AddToViewport();
	}
}

// Called to update wall weapon interaction widget if in sight of the player
void AAlexCharacter::CheckWallWeaponBuyInteractionWidget(AWallWeaponBuy* InWallWeaponBuy, UInteractionWidget* Widget) const
{
	if (InWallWeaponBuy == nullptr) return;
	AGun* Gun = Cast<AGun>(this->CurrentWeapon);
	if (Gun != nullptr)
	{
		Widget->SetActionTextColor(!(InWallWeaponBuy->GetWeaponClass() == Gun->GetClass()
			&& Gun->MaxAmmoForGun - Gun->GetCurrentTotalAmmo() <= 0));
		
		Widget->SetCostTextColor(InWallWeaponBuy->GetCost() <= this->CurrentPoints);
	}
	if (!Widget->IsInViewport())
	{
		Widget->AddToViewport();	
	}
}

// Called to update ammo item interaction widget if in sight of the player
void AAlexCharacter::CheckAmmoItemInteractionWidget(AAmmoItem* InAmmoItem, UInteractionWidget* Widget) const
{
	if (InAmmoItem == nullptr) return;
	UAmmoItemObject* AmmoItemObject = Cast<UAmmoItemObject>(InAmmoItem->GetItemObject());
	if (AmmoItemObject == nullptr) return;
	// Calculate the ammo amount lack of this ammo type in the inventory
	const int32 AmountLackInv = AmmoItemObject->MaxAmmoForType -
		this->Inventory->GetAmmoByWeaponType(AmmoItemObject->AmmoTypeByWeapon);
	Widget->SetActionTextColor(AmountLackInv > 0);
	if (!Widget->IsInViewport())
	{
		Widget->AddToViewport();	
	}
}

// Called to update weapon item interaction widget if in sight of the player
void AAlexCharacter::CheckWeaponItemInteractionWidget(AWeapon* InWeapon, UInteractionWidget* Widget)
{
	if (InWeapon == nullptr) return;

	if (!Widget->IsInViewport())
	{
		Widget->AddToViewport();
	}
}

// Called to update quest item interaction widget if in sight of the player
void AAlexCharacter::CheckQuestItemInteractionWidget(AQuestItem* InQuestItem, UInteractionWidget* Widget)
{
	if (InQuestItem == nullptr) return;

	if (!Widget->IsInViewport())
	{
		Widget->AddToViewport();
	}
}

void AAlexCharacter::CheckQuestItemsCraftingTableInteractionWidget(AQuestItemsCraftingTable* InQuestItemsCraftingTable,
	UInteractionWidget* Widget) const
{
	if (InQuestItemsCraftingTable == nullptr) return;

	// Check if method has to be updated after adding the shield comp to the character
	// Means to only check if the player owns a shield component
	if (QuestInventory != nullptr)
	{
		switch (InQuestItemsCraftingTable->GetQuestItemsCraftingTableType())
		{
			case QuestItemsCraftingTable_Shield:
			{
				if (Widget->GetActionText() != "You Already Own The " &&
					QuestInventory->GetQuestItemsByType(Shield).Num() > 0)
				{
					Widget->SetActionText(FText::FromString("You Already Own The "));
					Widget->SetActionTextColor(false);
				}
				else if (QuestInventory->GetQuestItemsByType(Shield).Num() == 0)
				{
					const FString ActionText = Widget->GetActionText();
					if (ActionText != "You Do Not Have All The Required Parts!" &&
						QuestInventory->GetQuestItemsByType(ShieldPart).Num() <
						QuestHandlerComp->GetShieldQuestStruct().PartsRequired)
					{
						Widget->SetActionText(FText::FromString("You Do Not Have All The Required Parts!"));
						Widget->SetObjectNameText(FText::FromString(""));
						Widget->SetActionTextColor(false);
					}
					else if (ActionText != InQuestItemsCraftingTable->GetWidgetActionText().ToString()
						&& QuestInventory->GetQuestItemsByType(ShieldPart).Num() ==
						QuestHandlerComp->GetShieldQuestStruct().PartsRequired)
					{
						Widget->SetActionText(InQuestItemsCraftingTable->GetWidgetActionText());
						Widget->SetObjectNameText(InQuestItemsCraftingTable->GetWidgetObjectNameText());
						Widget->SetActionTextColor(true);
					}
				}
			}
			default: ;
		}
	}

	if (!Widget->IsInViewport() && !InQuestItemsCraftingTable->GetIsInUse())
	{
		Widget->AddToViewport();
	}
	else if (InQuestItemsCraftingTable->GetIsInUse() && Widget->IsInViewport())
	{
		Widget->RemoveFromViewport();
	}
}


// Called to update door interaction widget if in sight of the player
void AAlexCharacter::CheckDoorInteractionWidget(ARegularDoor* InDoor, UInteractionWidget* Widget) const
{
	if (InDoor == nullptr) return;
	
	Widget->SetCostTextColor(InDoor->GetCost() <= this->CurrentPoints);
	if (!InDoor->IsShowWidget())
	{
		if (Widget->IsInViewport())
		{
			Widget->RemoveFromViewport();
		}
		return;
	}
	if (!Widget->IsInViewport())
	{
		Widget->AddToViewport();	
	}
}

// Called to update power stone interaction widget if in sight of the player
void AAlexCharacter::CheckPowerStoneInteractionWidget(APowerStone* InPowerStone, UInteractionWidget* Widget) const
{
	if (InPowerStone == nullptr) return;

	Widget->SetActionTextColor(!this->PowerStonesByTypes.Contains(InPowerStone->GetPowerStoneType()));
	Widget->SetCostTextColor(InPowerStone->GetCost() <= this->CurrentPoints);

	if (!Widget->IsInViewport())
	{
		Widget->AddToViewport();
	}
}

// Called to update obstacle actor interaction widget if in sight of the player
void AAlexCharacter::CheckObstacleInteractionWidget(AObstacle* InObstacleActor, UInteractionWidget* Widget) const
{
	if (InObstacleActor == nullptr) return;

	Widget->SetCostTextColor(InObstacleActor->GetCost() <= this->CurrentPoints);

	if (!Widget->IsInViewport())
	{
		Widget->AddToViewport();
	}
}


/* Using weapons */

// Called when the player press the key for using weapon in slot number 1
void AAlexCharacter::UseWeaponSlot1()
{
	if (this->WeaponSlots[0] == nullptr) return;

	this->CurrentWeaponSlotIndex = 0;
	this->UseWeapon(this->WeaponSlots[0]);
}

// Called when the player press the key for using weapon in slot number 2
void AAlexCharacter::UseWeaponSlot2()
{
	if (this->WeaponSlots[1] == nullptr) return;

	this->CurrentWeaponSlotIndex = 1;
	this->UseWeapon(this->WeaponSlots[1]);
}

// Called when the player press the key for using weapon in slot number 3
void AAlexCharacter::UseWeaponSlot3()
{
	if (this->WeaponSlots[2] == nullptr) return;

	this->CurrentWeaponSlotIndex = 2;
	this->UseWeapon(this->WeaponSlots[2]);
}

// Called when the player press the key for using weapon in slot number 4
void AAlexCharacter::UseWeaponSlot4()
{
	if (this->WeaponSlots[3] == nullptr) return;

	this->CurrentWeaponSlotIndex = 3;
	this->UseWeapon(this->WeaponSlots[3]);
}

// Called to remove a weapon from a slot
void AAlexCharacter::RemoveWeaponFromSlot(const FName WeaponItemId)
{
	for (uint8 i = 0; i < this->WeaponSlots.Num(); i++)
	{
		if (this->WeaponSlots[i] != nullptr && WeaponItemId == this->WeaponSlots[i]->GetItemId())
		{
			this->WeaponSlots[i] = nullptr;
			return;
		}
	}
}


/******************************************/

	
/* Footsteps */

// Called to check the player's velocity for playing foot steps sounds and effects
void AAlexCharacter::CheckVelocityForFootsteps()
{
	// Current velocity of the player character
	const FVector CurrentVelocity = this->GetVelocity();
	FTimerManager& TimerManager = this->GetWorldTimerManager();

	// If player character is not in idle state
	if (CurrentVelocity.X > 2.0f || CurrentVelocity.X < -2.0f || CurrentVelocity.Y > 2.0f || CurrentVelocity.Y < -2.0f
		&& CurrentVelocity.Z > 0.0f)
	{
		// Check if can set timer to footsteps
		if (this->FootstepsDoOnce)
		{
			// Reset
			this->FootstepsDoOnce = false;
			// Get the surface type and play sfx & vfx based on the surface type.
			TimerManager.SetTimer(this->TimerHandle_ActivateFootstep, this,
				&AAlexCharacter::PlayFootstepEffects, this->FootstepInterval, true);
		}
	}
	// Else if player character velocity doesn't allow footstep	` | v | < 2 `
	else
	{
		// Clearing timer which triggers footsteps vfx & sfx
		TimerManager.ClearTimer(this->TimerHandle_ActivateFootstep);
		// Reset can set timer to footsteps to true
		this->FootstepsDoOnce = true;
	}
}

/* Player die/ hurt functions */

// Called once the player gets damaged
float AAlexCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	this->PlayTakeDamageSfx(DamageAmount);
	
	const float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	const float HealthTemp = this->CurrentHealth;
	// Decreasing the player current health by the damage amount
	this->CurrentHealth -= FMath::Min(DamageToApply, this->CurrentHealth);
	// If health changed then broadcast to update health widget
	if (HealthTemp != this->CurrentHealth)
	{
		this->OnPlayerCurrentHealthChanged.Broadcast();
	}
	// Adding a blood material on screen
	this->AddBloodScreen();
	if (DamageCauser != nullptr)
	{
		this->OnPlayerTakeDamageEvent(DamageCauser);
	}
	// Check if player health is 0
	if (this->IsDead())
	{
		// Called to let the player die then load back to main menu screen
		this->PlayerDie();
	}
	// Otherwise check if health is equal or below max health to show blood material on screen
	else if (this->CurrentHealth <= this->MaximumHealthToShowBloodScreen && this->HeartBeatAudioComp->Sound != nullptr)
	{
		// Calculating heart beat audio component volume target value
		this->HeartBeatVolMultTarget = FCString::Atof(*FString::Printf(TEXT("%.3f"), this->MaximumHeartBeatSfxVolMult *
				FMath::Max(1 - FCString::Atof(*FString::Printf(TEXT("%.3f"), this->CurrentHealth / this->MaximumHealthToShowBloodScreen)),
					this->MaximumHeartBeatSfxVolMult)));
		
		FTimerManager& TimerManager = this->GetWorldTimerManager();
		if (TimerManager.IsTimerActive(this->TimerHandle_IncreaseHeartBeatsSfxVolMult))
		{
			TimerManager.ClearTimer(this->TimerHandle_IncreaseHeartBeatsSfxVolMult);
		}
		// Check if heart beat audio component is not playing
		if (!this->HeartBeatAudioComp->IsPlaying())
		{
			// Play the heart beat audio sounds
			this->HeartBeatAudioComp->Play();
		}
		// Called to slowly increase the heart beat audio component volume
		this->IncreaseHeartBeatsSfxVolumeMultiplier();
	}
	/*
	 Check if player health > 0 & bigger than full health &
	 currently is not regaining health
	*/
	if (!this->IsDead() && this->CurrentHealth < this->FullHealth && !this->bRegainHealth)
	{
		// Setting regain health to true to be updated in tick() function
		this->bRegainHealth = true;
	}
	
	return DamageToApply;
}

// Called to play sfx when the player takes damage (voice sfx)
void AAlexCharacter::PlayTakeDamageSfx(const float Damage) const
{
	if (this->TakeDamageVoiceAudioComp->IsPlaying() || Damage < 10.0f) return;
	this->TakeDamageVoiceAudioComp->Play();
}

// Called to increase blood amounts of blood screen material
void AAlexCharacter::AddBloodScreen()
{
	this->BloodSpeed = 15.f;
	// Setting the desired value of blood amount (increase) 
	this->DesiredBloodAmount = FMath::Clamp(this->CurrentBloodAmount + 1.f, 0.f, this->MaxBloodAmount);
	if (this->PlayerController != nullptr)
	{
		const float CameraShakeScale = FMath::FRandRange(0.01f, 0.5f);
		// Playing a camera shake after blood amounts on screen updated in tick()
		this->PlayerController->ClientStartCameraShake(this->TakeDamageCameraShakeClass, CameraShakeScale);
	}
}

// Called to update the blood amounts of blood screen material - called in tick()
void AAlexCharacter::UpdateBloodScreen()
{
	if (this->PlayerCamera == nullptr) return;

	// Calculating the health clamped (float range from 0 to 60, e.g. health 0 will output 1, health 60 will output 0)
	const float HealthClamped = FMath::Clamp(UKismetMathLibrary::MapRangeClamped(this->CurrentHealth,
		0.f, this->MaximumHealthToShowBloodScreen, 1.f, 0.f), 0.f, 1.f);
	
	const float CameraPostProcessBlendWeight = FMath::Clamp(this->CurrentBloodAmount + HealthClamped, 0.f, 1.f);
	// Setting player camera pp bw to health clamped + current blood amount 
	this->PlayerCamera->SetPostProcessBlendWeight(CameraPostProcessBlendWeight);

	// Calculating current low health target value (0 health -> 3, 60 health -> 0)
	const float LowHealthTargetValue = FMath::Clamp(UKismetMathLibrary::MapRangeClamped(this->CurrentHealth,
		0.f, this->MaximumHealthToShowBloodScreen, 3.0, 0.f), 0.f, 3.f);

	// Reassign low health indicator in strong to ease out curve
	this->LowHealthIndicatorPower = FMath::FInterpTo(this->LowHealthIndicatorPower, LowHealthTargetValue,
		this->GetWorld()->GetDeltaSeconds(), 2.f);
	
	// Setting "power" value in the blood screen mat collection to the low health indicator value 
	UKismetMaterialLibrary::SetScalarParameterValue(this->GetWorld(), this->BloodScreenMatCollection,
		FName("Power"), this->LowHealthIndicatorPower);

	// Calculates blood amount in strong to ease out curve
	const float TempBloodAmount = FMath::FInterpTo(this->CurrentBloodAmount, this->DesiredBloodAmount,
		this->GetWorld()->GetDeltaSeconds(), this->BloodSpeed);
	
	// Reassign current blood amount
	this->CurrentBloodAmount = TempBloodAmount;

	// Setting "Blood splash" value in the blood screen mat collection to the current blood amount value
	UKismetMaterialLibrary::SetScalarParameterValue(this->GetWorld(), this->BloodScreenMatCollection,
		FName("BloodSplash"), this->CurrentBloodAmount);

	// Check if current blood amount almost equal 0
	if (FMath::IsNearlyEqual(this->CurrentBloodAmount, TempBloodAmount, 0.01f))
	{
		// Resets desired blood amount to 0
		this->DesiredBloodAmount = 0.f;
		// Resets blood amount interpolation speed to 3
		this->BloodSpeed = 3.f;
	}
}

// Called when the player health reaches down to 0
void AAlexCharacter::PlayerDie()
{
	// Setting player died to true
	this->bDied = true;
	// Health back to full to easily fade out blood screen effects
	this->CurrentHealth = this->FullHealth;
	// Stopping the heart beat from playing
	this->HeartBeatAudioComp->Stop();

	// Player is hidden in game in death scene
	this->SetActorHiddenInGame(true);
	// Check if player weapon is valid
	if (this->CurrentWeapon != nullptr)
	{
		// Player current weapon is hidden in death scene 
		this->CurrentWeapon->SetActorHiddenInGame(true);
	}
	// removing widgets from viewport (Hiding UI elements)	
	this->HideUserInterfaces();
	this->OnPlayerDied.Broadcast();
	
	this->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (this->PlayerController == nullptr || this->PlayerCamera == nullptr
		|| !IsValid(this->PlayerDeathActorClass))
	{
		return;
	}

	// Disable input from the player
	this->DisableInput(this->PlayerController);
	// Spawn death actor to simulate player fall down on ground
	APlayerDeathActor* PlayerDeathActor = Cast<APlayerDeathActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass
		(this->GetWorld(), this->PlayerDeathActorClass, this->PlayerCamera->GetComponentTransform(),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	PlayerDeathActor->Init(this->PlayerCamera->PostProcessSettings);
	// Death actor set to be hidden in game
	PlayerDeathActor->SetActorHiddenInGame(true);
	// finishing spawning death actor
	UGameplayStatics::FinishSpawningActor(PlayerDeathActor, this->PlayerCamera->GetComponentTransform());
	// Setting the player to view the death actor
	this->PlayerController->SetViewTargetWithBlend(PlayerDeathActor);
}

// Called to clear all user interfaces after death (actual event is in the blueprint's child of this class)
void AAlexCharacter::HideUserInterfaces_Implementation()
{
	TArray<AActor*> InteractableActorsFound;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AInteractable::StaticClass(), InteractableActorsFound);
	for (AActor* Actor : InteractableActorsFound)
	{
		if (const AInteractable* InteractableActor = Cast<AInteractable>(Actor))
		{
			UUserWidget* InteractionWidget = InteractableActor->GetInteractionWidget();
			if (InteractionWidget != nullptr && InteractionWidget->IsInViewport())
			{
				InteractionWidget->RemoveFromViewport();
			}
		}
	}
}

// Called to increase the heart beats sound component volume multiplier
void AAlexCharacter::IncreaseHeartBeatsSfxVolumeMultiplier()
{
	// Check if timer is not set
	if (!this->GetWorldTimerManager().IsTimerActive(this->TimerHandle_IncreaseHeartBeatsSfxVolMult))
	{
		// Setting a timer to call this method to increase heart beat sound volume 
		this->GetWorldTimerManager().SetTimer(this->TimerHandle_IncreaseHeartBeatsSfxVolMult, this, &AAlexCharacter::IncreaseHeartBeatsSfxVolumeMultiplier, 0.008, true);
	}
	/*
	  Check if target heart beat sound volume reached the target limit
	  & if the timer is set
	 */
	if (this->CurrentHeartBeatVolMult == this->HeartBeatVolMultTarget
		&& this->GetWorldTimerManager().IsTimerActive(this->TimerHandle_IncreaseHeartBeatsSfxVolMult))
	{
		// Clearing the timer for increasing heart beat sound volume
		this->GetWorldTimerManager().ClearTimer(this->TimerHandle_IncreaseHeartBeatsSfxVolMult);
	}
	// Reassign heart beat audio component volume
	this->CurrentHeartBeatVolMult = FMath::Min(this->HeartBeatVolMultTarget, this->CurrentHeartBeatVolMult + 0.005f);
	this->HeartBeatAudioComp->SetVolumeMultiplier(this->CurrentHeartBeatVolMult);
}

// Called to increase the player points
void AAlexCharacter::IncreasePlayerPoints(const int32 AdditionalPoints, const bool bPlaySound)
{
	// Adding points to the player
	this->CurrentPoints += AdditionalPoints;
	// Called to update player points in the gameplay widget
	this->OnPlayerPointsChanged.Broadcast(this->CurrentPoints, AdditionalPoints, true);
	
	if (this->MainPlayerState != nullptr)
	{
		// Updating player stats score (points)
		this->MainPlayerState->SetScore(this->CurrentPoints);
	}
}

// Called to decrease the player points
void AAlexCharacter::DecreasePlayerPoints(const int32 PointsToSub, const bool bPlaySound)
{
	// Decreasing player points
	this->CurrentPoints = FMath::Max(CurrentPoints - PointsToSub, 0);
	// Called to update player points in the gameplay widget 
	this->OnPlayerPointsChanged.Broadcast(this->CurrentPoints, PointsToSub, false);
	// Check if play purchase sfx is true & the sfx is valid
	if (bPlaySound && this->BuyingSfx != nullptr)
	{
		// Playing purchase sfx
		UGameplayStatics::PlaySound2D(this, this->BuyingSfx);
	}
	
	if (this->MainPlayerState != nullptr)
	{
		// Updating player stats score (points)
		this->MainPlayerState->SetScore(this->CurrentPoints);
	}
}

// Called when the player click to use an item from his inventory
void AAlexCharacter::UseItem(UItemObject* InItemObject)
{
	if (InItemObject == nullptr) return;

	if (this->CurrentItemObjectInUse != nullptr &&
		this->CurrentItemObjectInUse->GetItemId() == InItemObject->GetItemId()) return;
	
	// Check if item type is weapon
	if (InItemObject->GetItemType() == Item_Weapon)
	{
		// Called to use the weapon
		this->UseWeapon(InItemObject);
	}
}

// Called to release the current weapon from the player character hands
void AAlexCharacter::ReleaseWeapon()
{
	if (this->CurrentWeapon == nullptr) return;
	
	this->CurrentItemObjectInUse = nullptr;
	// Destroying the weapon in the player hands
	this->CurrentWeapon->Destroy();
	this->CurrentWeapon = nullptr;
	this->bHasWeapon = false;
	// Check if the player has a weapon in left hand
	if (this->LeftHandCurrentWeapon != nullptr)
	{
		// Destroying weapon in left hand
		this->LeftHandCurrentWeapon->Destroy();
		this->LeftHandCurrentWeapon = nullptr;
	}

	// Trying to update mystery boxes and wall weapon buy widgets
	if (this->MainGameState != nullptr)
	{
		this->MainGameState->UpdateMysteryBoxes_WallWeaponsBuyInteractionWidgets(
			nullptr, 0, 0);
	}
	
	this->OnLeaveCurrentWeaponDel.Broadcast();
}

// Called when the player uses a quest item (quest item) from his quest-items inventory
void AAlexCharacter::UseQuestItem(UItemObject* InQuestItemObject)
{
	//..
}

/*
  Called to use a weapon from the inventory component widget or
  the weapons selection wheel component widget
 */
void AAlexCharacter::UseWeapon(UItemObject* InWeaponItemObject)
{
	// Player cannot use a weapon while changing weapon or if it is the same as the current one
	if (this->bIsChangingWeapon || (this->CurrentItemObjectInUse != nullptr &&
			this->CurrentItemObjectInUse->GetItemId() == InWeaponItemObject->GetItemId())) return;

	// Cancel reloading if the player is
	if (this->IsReloading)
	{
		this->bCanRightHandAttack = false;
		this->IsReloading = false;
	}
	else if (this->LeftHandCurrentWeapon != nullptr && this->bIsLeftHandReloading)
	{
		this->bCanLeftHandAttack = false;
		this->bIsLeftHandReloading = false;
	}
	
	this->CanReload = false;
	this->LeftHandCanReload = false;
	this->WeaponClassOnUse = InWeaponItemObject->GetItemClass();
	this->ItemObjectOnUse = InWeaponItemObject;
	// Sets to true to play animation and notify at a point to spawn the weapon in the player hands 
	this->bIsChangingWeapon = true;
}

// Called to spawn a weapon and attach it in the player's hands
void AAlexCharacter::SpawnWeaponInHands()
{ // O(1)
	if (!IsValid(this->WeaponClassOnUse) || this->PlayerMesh == nullptr) return;

	// Release any item in the player hands if there's, before spawning new one
	if (this->CurrentItemObjectInUse != nullptr)
	{
		this->ReleaseWeapon();
	}
	// Initial weapon spawn parameters
	FVector PlayerLocation = this->GetActorLocation();
	PlayerLocation.Z -= 3000.f;
	const FTransform WeaponTransform = FTransform(PlayerLocation);
	// Spawn the weapon
	AWeapon* Weapon = Cast<AWeapon>(UGameplayStatics::BeginDeferredActorSpawnFromClass
		(this->GetWorld(), this->WeaponClassOnUse, WeaponTransform,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this));
	Weapon->SetIsInSpawningState(true);
	// Finish spawn the weapon
	UGameplayStatics::FinishSpawningActor(Weapon, WeaponTransform);
	AGun* Gun = Cast<AGun>(Weapon);
	// Check if the spawned weapon is a gun
	if (Gun != nullptr)
	{
		// Check if gun is a dual guns
		if (Gun->IsDualWeapons)
		{
			// Spawn 2nd gun (left hand gun)
			AWeapon* LeftHandWeapon = Cast<AWeapon>(UGameplayStatics::BeginDeferredActorSpawnFromClass
				(this->GetWorld(), this->WeaponClassOnUse, FTransform(),
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this));
			LeftHandWeapon->SetIsInSpawningState(true);
			// Finish spawn 2nd gun
			UGameplayStatics::FinishSpawningActor(LeftHandWeapon, WeaponTransform);
			// Another check if class of weapon is actually a gun for the 2nd gun
			AGun* LeftHandGun = Cast<AGun>(LeftHandWeapon);
			if (LeftHandGun != nullptr)
			{
				// The material around the item is not set for weapons items after spawning in hands
				LeftHandGun->GetSkeletalMeshComponent()->SetRenderCustomDepth(false);
				// Removing the static mesh of the gun's root component (skeletal mesh is used instead)
				LeftHandGun->GetStaticMeshComponent()->RemoveFromRoot();
				LeftHandGun->GetStaticMeshComponent()->DestroyComponent();
				/*
				  Resetting transform for the gun as transform default values are used in
				  the animation sequence if the gun
				 */
				LeftHandGun->GetSkeletalMeshComponent()->ResetRelativeTransform();
				// Attaching the gun to the player character skeletal mesh
				LeftHandGun->AttachToComponent(this->PlayerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale
				, LeftHandGun->GetPlayerWeaponSocketNameLeftHand());
				// The owner of the gun is the player character
				LeftHandGun->SetOwner(this);
				this->LeftHandCurrentWeapon = LeftHandGun;
				this->LeftHandCurrentWeapon->GetSphereComponent()->RemoveFromRoot();
				this->LeftHandCurrentWeapon->GetSphereComponent()->DestroyComponent();
				this->LeftHandCanReload = true;
				this->LeftHandCurrentWeapon->IsLeftGun = true;
			}
			Gun->GetStaticMeshComponent()->RemoveFromRoot();
			Gun->GetStaticMeshComponent()->DestroyComponent();
		}
		/* Right hand gun setups */
		Gun->GetSkeletalMeshComponent()->SetRenderCustomDepth(false);
		Gun->GetSphereComponent()->RemoveFromRoot();
		Gun->GetSphereComponent()->DestroyComponent();
		Gun->GetSkeletalMeshComponent()->ResetRelativeTransform();
		Gun->AttachToComponent(this->PlayerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale
		, Gun->GetPlayerWeaponSocketName());
		Gun->SetOwner(this);
	}
	// Otherwise means it's a melee, currently there's only guns in the game!
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Finally melee weapons! not gun has been spawned!"));
		return;
	}

	this->CurrentItemObjectInUse = this->ItemObjectOnUse;
	this->ItemObjectOnUse = nullptr;
	
	this->CurrentWeapon = Weapon;
	this->CurrentWeapon->SetItemObject(this->CurrentItemObjectInUse);
	this->bIsChangingWeapon = false;
	this->bHasWeapon = true;
	this->CanReload = true;

	this->SpawnWeaponInHands_Part2(Gun);
}

void AAlexCharacter::SpawnWeaponInHands_Part2(AGun* Gun)
{ // O(N)
	if (Gun != nullptr)
	{
		if (this->LeftHandCurrentWeapon != nullptr)
		{
			this->LeftHandCurrentWeapon->SetItemObject(this->CurrentWeapon->GetItemObject());
			if (AGun* LeftHandGun = Cast<AGun>(this->LeftHandCurrentWeapon))
			{
				LeftHandGun->InitializeAmmo();
			}
		}
		Gun->InitializeAmmo();
		this->ReportNoise(this->GetActorLocation(), 0.9f);
	}

	if (this->CurrentWeapon != nullptr)
	{
		this->MaximumWalkingSpeedWithWeapon = this->CurrentWeapon->PlayerMaximumWalkingSpeed;
		this->NormalWalkingSpeedWithWeapon = this->CurrentWeapon->PlayerNormalWalkingSpeed;
		
		// If the player owns a speedy power stone then the walking speeds with the current weapon will increased
		if (this->PowerStonesByTypes.Contains(PowerStone_Speedy))
		{
			this->MaximumWalkingSpeedWithWeapon *= this->SpeedyPowerStonePowerMultiplier;
			this->NormalWalkingSpeedWithWeapon *= this->SpeedyPowerStonePowerMultiplier;
		}
	}
	
	if (this->MainGameState != nullptr)
	{
		const UClass* CurrentWeaponClass = nullptr;
		int32 AmmoLack = 0, MaxAmmoForGun = 0;
		if (this->CurrentWeapon != nullptr)
		{
			CurrentWeaponClass = this->CurrentWeapon->GetClass();
			if (const AGun* CurrentGun = Cast<AGun>(CurrentWeapon))
			{
				MaxAmmoForGun = CurrentGun->MaxAmmoForGun;
				AmmoLack = MaxAmmoForGun - (CurrentGun->GetCurrentTotalAmmo() + CurrentGun->GetCurrentAmmo());
			}
		}
		this->MainGameState->UpdateMysteryBoxes_WallWeaponsBuyInteractionWidgets(CurrentWeaponClass, MaxAmmoForGun, AmmoLack);
	}
	
	this->OnUseWeaponDel.Broadcast();
}

// Called to attach shield to the player
void AAlexCharacter::AttachShieldToPlayer(UItemObject* InItemObject)
{
	if (!IsValid(ShieldActorClass)) return;

	const FVector Location(0.f, 0.f, -2000.f);
	const FRotator Rotation(0.f, 0.f, 0.f);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (AShieldActorComponent* Shield = Cast<AShieldActorComponent>(GetWorld()->SpawnActor(ShieldActorClass, &Location,
		&Rotation, SpawnParameters)))
	{
		Shield->SetItemObject(InItemObject);
		ShieldComp = Shield;
		ShieldComp->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale,
			FName("Shield_Socket"));

		SetHasShield(true);

		if (LaughSfx != nullptr && FMath::RandRange(0, 10) <= 2)
		{
			UGameplayStatics::PlaySound2D(this, LaughSfx, 1.f, 1.f,
				0.f, nullptr, this, false);
		}
	}
}

// Called to adjust the player's velocity while holding down ads button
void AAlexCharacter::AdjustSpeedWhileAds()
{
	if (!this->bIsAiming || this->CurrentWeapon == nullptr) return;

	// Reassign target velocity to walking speed while aiming down sight
	this->TargetVelocity = this->CurrentWeapon->PlayerWalkingSpeedAds;
	
	this->GetCharacterMovement()->MaxWalkSpeed < this->TargetVelocity ? this->TargetVelocityUpdateStep = 1 :
			this->TargetVelocityUpdateStep = -1;
	
	if (this->GetWorldTimerManager().IsTimerActive(this->TimerHandle_UpdateCharacterVelocity))
	{
		this->GetWorldTimerManager().ClearTimer(this->TimerHandle_UpdateCharacterVelocity);
	}
	
	this->GetWorldTimerManager().SetTimer(this->TimerHandle_UpdateCharacterVelocity, this,
		&AAlexCharacter::UpdateCharacterVelocity, 0.001, true);
}

// Called to re-adjust player's velocity after releasing the ads button 
void AAlexCharacter::AdjustSpeedAfterReleaseAds()
{
	if (this->bSprintNotified)
	{
		this->CurrentWeapon != nullptr ? this->TargetVelocity = this->MaximumWalkingSpeedWithWeapon
			: this->TargetVelocity = this->MaximumWalkingSpeed;
	}
	else if (this->CurrentWeapon != nullptr)
	{
		this->TargetVelocity = this->NormalWalkingSpeedWithWeapon;
	}
	else
	{
		this->TargetVelocity = this->NormalWalkingSpeed;
	}

	if (this->GetWorldTimerManager().IsTimerActive(this->TimerHandle_UpdateCharacterVelocity))
	{
		this->GetWorldTimerManager().ClearTimer(this->TimerHandle_UpdateCharacterVelocity);
	}
	
	this->GetWorldTimerManager().SetTimer(this->TimerHandle_UpdateCharacterVelocity, this,
		&AAlexCharacter::UpdateCharacterVelocity, 0.001, true);
}

// Called to disable the movements/ combat/ interactions actions by the player
void AAlexCharacter::InSwitchToWidgetInputs()
{
	this->CanReload = false;
	this->bCanRightHandAttack = false;
	this->LeftHandCanReload = false;
	this->bCanLeftHandAttack = false;
}

// Called to enable the movements/ combat/ interactions actions by the player
void AAlexCharacter::InSwitchToInGameInputs()
{
	this->CanReload = true;
	this->bCanRightHandAttack = true;
	this->LeftHandCanReload = true;
	this->bCanLeftHandAttack = true;
}


/******************************************/


/* Getters */

// Method returns the player camera component
UCameraComponent* AAlexCharacter::GetPlayerCamera() const
{
	return this->PlayerCamera;
}

/*
  Method returns the current item's item object that
  is currently being used by the player
 */
UItemObject* AAlexCharacter::GetCurrentItemObjectInUse() const
{
	return this->CurrentItemObjectInUse;
}

// Method returns the current weapon the player is using
AWeapon* AAlexCharacter::GetCurrentWeapon() const
{
	return this->CurrentWeapon;
}

// Method returns the left hand weapon the player is using
AWeapon* AAlexCharacter::GetLeftHandCurrentWeapon() const
{
	return this->LeftHandCurrentWeapon;
}

// Method returns the player's inventory component
UInventory* AAlexCharacter::GetPlayerInventory() const
{
	return this->Inventory;
}

// Method returns the player's weapons selection wheel component
UWeaponsSelectionWheel* AAlexCharacter::GetWeaponsWheel() const
{
	return this->WeaponsWheel;
}

// Method returns the player's quest inventory component
UQuestInventory* AAlexCharacter::GetPlayerQuestInventory() const
{
	return this->QuestInventory;
}

// Method returns the player's shield actor component
AShieldActorComponent* AAlexCharacter::GetPlayerShieldComponent() const
{
	return this->ShieldComp;
}

// Method returns whether the player is dead or not
bool AAlexCharacter::IsDead() const
{
	return this->CurrentHealth <= 0 || this->bDied;
}

// Method returns the current points amount with the player 
int32 AAlexCharacter::GetPlayerCurrentPoints() const
{
	return this->CurrentPoints;
}

// Method returns all power stones with the player
TArray<APowerStone*> AAlexCharacter::GetPlayerPowerStones() const
{
	return this->PowerStones;
}

// Method returns the types of all the power stones with the player 
TArray<EPowerStoneType> AAlexCharacter::GetPlayerPowerStonesByTypes() const
{
	return this->PowerStonesByTypes;
}

// Method returns whether the player is attacking or not
bool AAlexCharacter::GetIsPlayerAttacking() const
{
	return this->IsAttacking;
}

// Method returns ammo items classes collection
TMap<TEnumAsByte<EWeaponType>, TSubclassOf<AAmmoItem>> AAlexCharacter::GetAmmoItemsClasses() const
{
	return this->AmmoItemsClasses;
}

// Method returns weapon slots with the player
TArray<UWeaponItemObject*> AAlexCharacter::GetWeaponSlots()
{
	return this->WeaponSlots;
}

// Method returns whether the player has a shield or not
bool AAlexCharacter::HasShield() const
{
	return this->bHasShield;
}

USoundCue* AAlexCharacter::GetLaughingSoundCue() const
{
	return LaughSfx;
}


/* Setters */

/*
  Method to set - player is attacking property value
  called to handle some other functionalities that
  requires this property to be set to false
 */
void AAlexCharacter::SetIsPlayerAttacking(const bool InIsAttacking)
{
	this->IsAttacking = InIsAttacking;
}

// Method to set - player right hand can attack property value
void AAlexCharacter::SetCanRightHandAttack(const bool InCanAttack)
{
	this->bCanRightHandAttack = InCanAttack;
}

// Method to set - player left hand is attacking property value (left hand weapon - dual weapons)
void AAlexCharacter::SetLeftHandIsPlayerAttacking(const bool InIsAttacking)
{
	this->bIsLeftHandAttacking = InIsAttacking;
}

void AAlexCharacter::SetWeaponSlots(const TArray<UWeaponItemObject*> InWeaponSlots)
{
	this->WeaponSlots = InWeaponSlots;
}

void AAlexCharacter::SetHasShield(const bool InbHasShield)
{
	this->bHasShield = InbHasShield;
}
