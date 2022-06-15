// Mahdi Abu Hamida - Final University Project (2022)
        
#pragma once
        
#include "CoreMinimal.h"
#include "BloodySoulsTown/InteractableActors/PowerStones/PowerStoneType.h"
#include "BloodySoulsTown/InteractableActors/Items/Weapons/WeaponType.h"
#include "GameFramework/Character.h"
#include "AlexCharacter.generated.h"
        
        
/* Forward Classes Declaration */
        
class UInventory;
class UWeaponsSelectionWheel;
class AInteractable;
class AItem;
class UItemObject;
class AWeapon;
class UWeaponItemObject;
class UCurveFloat;
class UCameraComponent;
        
        
/******************************************/
        
        
/* Delegate Dispatchers Declaration */
        
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseWeaponDel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeaveCurrentWeaponDel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartFireDel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopFireDel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnForceReleaseAds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerCurrentHealthChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerPointsChanged, int32, NewPoints, int32, Difference, bool, bIncreased);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerEquipsPowerStone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNoMoreAmmoAllowedOfType, TEnumAsByte<EWeaponType>, AmmoType);
        
        
/******************************************************************************************/
        
        
UCLASS()
class BLOODYSOULSTOWN_API AAlexCharacter final : public ACharacter
{
	GENERATED_BODY()
        
public:
	// Sets default values for this character's properties
	AAlexCharacter();
       
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    
    	
    /* Event dispatchers delegates */
    
    // When the current player's weapon released from the player hands
    UPROPERTY(BlueprintAssignable)
    FOnLeaveCurrentWeaponDel OnLeaveCurrentWeaponDel;
        	
    // When the player start firing with a gun (not melee weapon)
    UPROPERTY(BlueprintAssignable)
    FOnStartFireDel OnStartFireDel;
    	
    // When the player stop firing with a gun (not melee weapon)
    UPROPERTY(BlueprintAssignable)
    FOnStopFireDel OnStopFireDel;
    	
    // When the player dies
    UPROPERTY(BlueprintAssignable)
    FOnPlayerDied OnPlayerDied;
    	
    // When the player's points increases
    UPROPERTY(BlueprintAssignable)
    FOnPlayerPointsChanged OnPlayerPointsChanged;
    	
    // When the player current health changes
    UPROPERTY(BlueprintAssignable)
    FOnPlayerCurrentHealthChanged OnPlayerCurrentHealthChanged;
    	
    // When the player use a weapon
    UPROPERTY(BlueprintAssignable)
    FOnUseWeaponDel OnUseWeaponDel;
    
    // When the player gets a new power stone
    UPROPERTY(BlueprintAssignable)
    FOnPlayerEquipsPowerStone OnEquipPowerStoneDel;

	// When the player try to get ammo item but no more is allowed of the ammo type
	UPROPERTY(BlueprintAssignable)
	FOnNoMoreAmmoAllowedOfType OnNoMoreAmmoAllowedOfType;
    
    
    /* Timers for fire/ reload weapon */
    
    // Timer handle for activating fire at a time rate which is the fire rate of a weapon
    FTimerHandle TimerHandle_ReFire;
    
    // Timer handle for reloading
    FTimerHandle TimerHandle_Reloading;
    
    // Timer handle for activating fire with the left hand weapon
    FTimerHandle TimerHandle_LeftReFire;
    
    // Timer handle for reloading with the left hand weapon
    FTimerHandle TimerHandle_LeftReloading;
    
    	
    /* Main functions */
    
    // Called to disable the movements/ combat/ interactions actions by the player
    UFUNCTION(BlueprintCallable)
    void InSwitchToWidgetInputs();
    
    // Called to enable the movements/ combat/ interactions actions by the player
    UFUNCTION(BlueprintCallable)
    void InSwitchToInGameInputs();
    
    // Called when the player click to use an item from his inventory
    UFUNCTION(BlueprintCallable)
    void UseItem(UItemObject* InItemObject);
    
    // Called to release the current weapon from the player character hands
    void ReleaseWeapon();
    
    // Player stops attacking
    void StopAttack();
    
    /*
      Called to start attacking once the player hit once/ holds down the attack button
	  (gamepad left trigger)/ (mouse right button).
      when the player has a dual weapons it activates the right hand weapon attack (mouse)
      or left hand weapon attack (gamepad)
      this method activates ads single attack
     */
    UFUNCTION(BlueprintCallable)
    void AdsStartAttack();
    
    /*
      Called to stop attacking once the player release the when having dual weapons
      if using a mouse then the weapon in the right hand will stop attacking
      if using a gamepad then the weapon in the left hand will stop attacking
     */
    UFUNCTION(BlueprintCallable)
    void AdsStopAttack();
    
    /*
      Event called to notify the child blueprint class to report for noise
      whenever the player fire with a gun or pick up items or reload a gun
      or when he's moving
     */
    UFUNCTION(BlueprintImplementableEvent)
    void ReportNoise(FVector NoiseSourceLocation, float SourceLoudness);
    
    // When the player takes a damage
    virtual float TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent,
    	AController* EventInstigator, AActor *DamageCauser) override;

	// Called to play sfx when the player takes damage (voice sfx)
	void PlayTakeDamageSfx(const float Damage) const;
    
    /*
      Called to fire some actions in the blueprint class
      this function event is only used in the blueprint child class
     */
    UFUNCTION(BlueprintImplementableEvent)
    void OnPlayerTakeDamageEvent(AActor* DamageCauser);
    
    // When the player reloads (public method as it used in the gun class as well)
    void Reload();
    
    
    /* Using weapons */
    
    // Called when the player press the key for using weapon in slot number 1
    void UseWeaponSlot1();
    
    // Called when the player press the key for using weapon in slot number 2
    void UseWeaponSlot2();
    
    // Called when the player press the key for using weapon in slot number 3
    void UseWeaponSlot3();
    
    // Called when the player press the key for using weapon in slot number 4
    void UseWeaponSlot4();

	// Called to remove a weapon from a slot
	void RemoveWeaponFromSlot(const FName WeaponItemId);
    
    // Called to increase the player points
    void IncreasePlayerPoints(const int32 AdditionalPoints, const bool bPlaySound);
    
    // Called to decrease the player points
    void DecreasePlayerPoints(const int32 PointsToSub, const bool bPlaySound);
    
    
    /* Getters */
    
    // Method returns the player camera component
    UFUNCTION(BlueprintPure)
    UCameraComponent* GetPlayerCamera() const;
    
    /*
      Method returns the current item's item object that
      is currently being used by the player
     */
    UFUNCTION(BlueprintCallable)
    UItemObject* GetCurrentItemObjectInUse() const;
    
    // Method returns the current weapon the player is using
    UFUNCTION(BlueprintCallable)
    AWeapon* GetCurrentWeapon() const;
    
    // Method returns the left hand weapon the player is using
    UFUNCTION(BlueprintCallable)
    AWeapon* GetLeftHandCurrentWeapon() const;
    
    // Method returns the player's inventory component
    UFUNCTION(BlueprintCallable)
    UInventory* GetPlayerInventory() const;
    
    // Method returns the player's weapons selection wheel component
    UFUNCTION(BlueprintCallable)
    UWeaponsSelectionWheel* GetWeaponsWheel() const;

	// Method returns the player's quest inventory component
	UFUNCTION(BlueprintPure)
	class UQuestInventory* GetPlayerQuestInventory() const;

	// Method returns the player's shield actor component
	class AShieldActorComponent* GetPlayerShieldComponent() const;
    
    // Method returns whether the player is dead or not
    UFUNCTION(BlueprintPure)
    bool IsDead() const;
    
    // Method returns the current points amount with the player 
    UFUNCTION(BlueprintPure)
    int32 GetPlayerCurrentPoints() const;
    
    // Method returns all power stones with the player
    UFUNCTION(BlueprintPure)
    TArray<class APowerStone*> GetPlayerPowerStones() const; 
    
    // Method returns the types of all the power stones with the player 
    TArray<EPowerStoneType> GetPlayerPowerStonesByTypes() const;
    
    // Method returns whether the player is attacking or not
    bool GetIsPlayerAttacking() const;
    
    // Method returns ammo items classes collection
    TMap<TEnumAsByte<EWeaponType>, TSubclassOf<class AAmmoItem>> GetAmmoItemsClasses() const;

	// Method returns weapon slots with the player
	TArray<UWeaponItemObject*> GetWeaponSlots();

	// Method returns whether the player has a shield or not
	UFUNCTION(BlueprintPure)
	bool HasShield() const;

	class USoundCue* GetLaughingSoundCue() const;
    
    
    /* Setters */
    
    /*
      Method to set - player is attacking property value
      called to handle some other functionalities that
      requires this property to be set to false
     */
    void SetIsPlayerAttacking(const bool InIsAttacking);
    
    // Method to set - player right hand can attack property value
    void SetCanRightHandAttack(const bool InCanAttack);
    
    // Method to set - player left hand is attacking property value (left hand weapon - dual weapons)
    void SetLeftHandIsPlayerAttacking(const bool InIsAttacking);

	void SetWeaponSlots(const TArray<UWeaponItemObject*> InWeaponSlots);

	void SetHasShield(const bool InbHasShield);
	
    /* Main properties */
    
    // Player current full health
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
    float FullHealth;
    
    // Player full health while owning "double souls power stone"
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
    float DoubleSoulsFullHealth;
    
    // Player current health
    UPROPERTY(BlueprintReadWrite, Category = "Health")
    float CurrentHealth;
    
    // Health step value to regain in tick func
    UPROPERTY(EditDefaultsOnly, Category = "Health")
    float HealthRegainStep = 0.05;

	UPROPERTY()
	bool bDied = false;
    
    // Whether the player has a weapon or not
    UPROPERTY(BlueprintReadOnly)
    bool bHasWeapon = false;
    
    // Whether the player is changing between weapons or not
    UPROPERTY(BlueprintReadOnly)
    bool bIsChangingWeapon = false;
    
    // Whether the player can attack or not
    UPROPERTY(BlueprintReadWrite)
    bool bCanRightHandAttack = true;
    
    // Whether the player's left hand can attack or not (left hand weapon - dual weapons)
    UPROPERTY(BlueprintReadWrite)
    bool bCanLeftHandAttack = true;
    
    // Whether the player is reloading a weapon or not 
    UPROPERTY(BlueprintReadWrite)
    bool IsReloading = false;
    
    // Whether the player is attacking with a weapon in the left hand or not (left hand weapon - dual weapons)
    UPROPERTY(BlueprintReadWrite)
    bool bIsLeftHandAttacking = false;
    
    // Whether the player is reloading a weapon in the left hand or not (left hand weapon - dual weapons)
    UPROPERTY(BlueprintReadWrite)
    bool bIsLeftHandReloading = false;
    
    // Whether the player is using a gamepad or not (value determined only in the custom controller blueprint class)
    UPROPERTY(BlueprintReadWrite)
    bool bIsGamepadKey = false;
    
    // Whether the player aiming with a weapon or not (holding down ads button or key)
    UPROPERTY(BlueprintReadWrite)
    bool bIsAiming = false;
    
    
    /* Power stones */
    
    // Value to be multiplied by the maximum character velocity when buying a speedy power stone
    UPROPERTY(EditDefaultsOnly, Category = "Power Stones")
    float SpeedyPowerStonePowerMultiplier;
    
    // Current power stones collection (only stone types - not actual actor pointers) with the player
    TArray<EPowerStoneType> PowerStonesByTypes;
    
    // Current power stones by references
    TArray<APowerStone*> PowerStones;
    
    
    /* Player weapons slots */

	UPROPERTY()
	TArray<UWeaponItemObject*> WeaponSlots;
    
    // The slot index of the current weapon in use (valid from 0 to 3)
    int32 CurrentWeaponSlotIndex = -1;
    
    /*
      Whether the player is near a wall or not 
      used to modify player animation - to avoid colliding with walls while having a weapon)
      as the player's weapon in hands is under the structure of the mesh of the player character
      means that the player's capsule component or the mesh phys asset are not responsible
      of the weapon's collision length towards other 3D objects with a blocking collision
     */
    UPROPERTY(BlueprintReadOnly)
    bool bIsNearWall;
    
    // Whether the player can do an ads with the weapon or not
    UPROPERTY(BlueprintReadWrite)
    bool bCanAds = true;
    	
protected:
	// Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    // Called to spawn a weapon and attach it in the player's hands
    UFUNCTION(BlueprintCallable)
    void SpawnWeaponInHands();
	void SpawnWeaponInHands_Part2(class AGun* Gun);

	// Called to attach shield to the player
	void AttachShieldToPlayer(UItemObject*);
    
    // Called to clear all user interfaces after death (actual event is in the blueprint's child of this class)
    UFUNCTION(BlueprintNativeEvent)
    void HideUserInterfaces();
    
    // Called to adjust the player's velocity while holding down ads button
    UFUNCTION(BlueprintCallable)
    void AdjustSpeedWhileAds();
    
    // Called to re-adjust player's velocity after releasing the ads button 
    UFUNCTION(BlueprintCallable)
    void AdjustSpeedAfterReleaseAds();


	UPROPERTY(EditDefaultsOnly, Category = "Classes References")
	TSubclassOf<AShieldActorComponent> ShieldActorClass;

	
    /* Footsteps */
    
    // Blueprint event called to play a foot step effects
    UFUNCTION(BlueprintImplementableEvent)
    void PlayFootstepEffects();
	
	
    /* Sounds */
    
    UPROPERTY(VisibleDefaultsOnly)
    UAudioComponent* HeartBeatAudioComp;

	UPROPERTY(VisibleDefaultsOnly)
	UAudioComponent* TakeDamageVoiceAudioComp;
    
    UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	class USoundCue* BuyingSfx;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* NoMoreAmmoOfTypeSfx;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* LaughSfx;

	
    /* Player walking speeds */
    
    // Player maximum walking speed (sprint)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movements")
    float MaximumWalkingSpeed = 400.f;
    
    // Player normal walking speed 
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movements")
    float NormalWalkingSpeed = 100.f;
    
    // Player maximum walking speed while having a weapon in hands
    UPROPERTY(BlueprintReadOnly)
    float MaximumWalkingSpeedWithWeapon;
    
    // Player normal walking speed while having a weapon in hands
    UPROPERTY(BlueprintReadOnly)
    float NormalWalkingSpeedWithWeapon;
    
    
    /******************************************/
    
    
    // Whether the player is attacking or not
    UPROPERTY(BlueprintReadOnly)
    bool IsAttacking = false;
    
    // Delegate called to force release the ads
    UPROPERTY(BlueprintAssignable)
    FOnForceReleaseAds OnForceReleaseAds;
    
    
    /*** Gamepad properties ***/
    
    // Horizontal (x axis) speed for gamepad (player look right-left)
    UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Controls|Gamepad")
    float GamepadSensitivityX = 40.f;
    
    // Vertical (y axis) speed for gamepad (player look up-down)
    UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Controls|Gamepad")
    float GamepadSensitivityY = 40.f;
    
    // Gamepad ads sensitivity multiplier for weapons with a low zoom sight
    UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Controls|Gamepad")
    float GamepadLowZoomSenMultiplier = 1.f;
    
    // Gamepad ads sensitivity multiplier for weapons with a high zoom sight
    UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Controls|Gamepad")
    float GamepadHighZoomSenMultiplier = 1.f;
	
    // Speed of aim assist (Higher = Stronger)
    UPROPERTY(EditDefaultsOnly, Category = "Controls|Gamepad")
    float InterpolationSpeedForMovingAim = 0.2f;
    
    // Speed of aim assist while aiming
    UPROPERTY(EditDefaultsOnly, Category = "Controls|Gamepad")
    float InterpolationSpeedForMovingAimWhileAds = 0.4f;
    
    
    /* Mouse properties */
    
    // Horizontal (x axis) speed for mouse (player look right-left) (14)
    UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Controls|Mouse")
    float MouseSensitivityX = 14.f;
    
    // Vertical (y axis) speed for mouse (player look up-down) (14)
    UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Controls|Mouse")
    float MouseSensitivityY = 14.f;
    
    // Mouse ads sensitivity multiplier for weapons with a low zoom sight (1)
    UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Controls|Mouse")
    float MouseLowZoomSenMultiplier = 1.f;
    
    // Mouse ads sensitivity multiplier for weapons with a high zoom sight (1)
    UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Controls|Mouse")
    float MouseHighZoomSenMultiplier = 1.f;
    
    
    /*** Camera shakes ***/
    
    // Camera shake for when the player is in an idle state (~ 0 v)
    UPROPERTY(EditDefaultsOnly, Category = "CameraShakes")
    TSubclassOf<UMatineeCameraShake> IdleCameraShakeClass;
    
    // Camera shake for when the player is in a walking state (~ normal player walking velocity)
    UPROPERTY(EditDefaultsOnly, Category = "CameraShakes")
    TSubclassOf<UMatineeCameraShake> WalkingCameraShakeClass;
    
    // Camera shake for when the player is in a running state (~ maximum player walking velocity)
    UPROPERTY(EditDefaultsOnly, Category = "CameraShakes")
    TSubclassOf<UMatineeCameraShake> RunningCameraShakeClass;
    
    // Camera shake for when the player is aiming down sights
    UPROPERTY(EditDefaultsOnly, Category = "CameraShakes")
    TSubclassOf<UMatineeCameraShake> AdsCameraShake;
    
    // Camera shake for when the player takes a damage
    UPROPERTY(EditDefaultsOnly, Category = "CameraShakes")
    TSubclassOf<UMatineeCameraShake> TakeDamageCameraShakeClass;
    
    
    /*** Blood screen ***/
    
    // Current blood amount opacity shows on screen
    UPROPERTY(EditDefaultsOnly, Category = "BloodScreen")
    float CurrentBloodAmount = 0.f;
    
    // Maximum blood amount to show (2)
    UPROPERTY(EditDefaultsOnly, Category = "BloodScreen")
    float MaxBloodAmount = 2.f;
    
    // Minimum blood amount to show (0)
    UPROPERTY(EditDefaultsOnly, Category = "BloodScreen")
    float MinBloodAmount = 0.f;
    
    // The current desired blood amount to show
    UPROPERTY(EditDefaultsOnly, Category = "BloodScreen")
    float DesiredBloodAmount = 0.f;
    
    // Blood opacity change speed (5)
    UPROPERTY(EditDefaultsOnly, Category = "BloodScreen")
    float BloodSpeed = 5.f;
    
    // The player's health value which will allow the blood to show on screen (60)
    UPROPERTY(EditDefaultsOnly, Category = "BloodScreen")
    float MaximumHealthToShowBloodScreen = 60.f;
    
    // Material collection of the blood screen 
    UPROPERTY(EditDefaultsOnly, Category = "BloodScreen")
    UMaterialParameterCollection* BloodScreenMatCollection;
    
    float LowHealthIndicatorPower = 0.f;
    
    
    /******************************************/
    
    
    // Current points with the player (begins with 500 points)
    UPROPERTY(EditDefaultsOnly)
    int32 CurrentPoints = 500;
    
    
    /*** Footsteps ***/
    
    // Need to change this to be according on the player character velocity ***
    UPROPERTY(EditDefaultsOnly, Category = "Footsteps")
    float FootstepInterval = 0.5f;
    
    
    /* Heartbeat sound effect */
    
    UPROPERTY(EditDefaultsOnly, Category = "Player Character Sounds|Heart Beat")
    float MaximumHeartBeatSfxVolMult = 0.2f;
    
private:
    // AI perception stimuli source component
    UPROPERTY(EditDefaultsOnly)
    class UAIPerceptionStimuliSourceComponent* PerceptionStimuliSourceComp;
    
    // Main game mode reference
    UPROPERTY()
    class AMainGameMode* MainGameMode; 
    
    // Main game instance reference
    UPROPERTY()
    class UMyGameInstance* MyGameInstance;
    
    // Main game state reference
    UPROPERTY()
    class AMainGameState* MainGameState;

	// Main player state
	UPROPERTY()
	class AMainPlayerState* MainPlayerState;
    
    // Player controller reference
    UPROPERTY()
    APlayerController* PlayerController;
    
    
    /******************************************/
    
    
    /*** Axis input functions ***/
    
    // Called to update the player character forward vector value (move forward or backwards)
    void MoveForward(float AxisValue);
    
    // Called to update the player character right vector value (move right or left)
    void MoveRight(float AxisValue);
    
    // Called to update the player character pitch control rotation (mouse y-axis)
    void LookUp(float AxisValue);
    
    // Called to update the player character yaw control rotation (mouse x-axis)
    void LookRight(float AxisValue);
    
    // Called to update the player character pitch control rotation (gamepad right stick y-axis)
    void LookUpRate(float AxisValue);
    
    // Called to update the player character yaw control rotation (gamepad right stick x-axis)
    void LookRightRate(float AxisValue);
    
    
    /* Sprinting */
    
    // Called when the player holds down the sprint button/ key
    void SprintPressed();
    
    // Called when the player releases the sprint button/ key
    void SprintReleased();
    
    
    /******************************************/
    
    
    // Called to update the player character velocity
    UFUNCTION()
    void UpdateCharacterVelocity();
    
    // Timer handle for updating the player character velocity
    FTimerHandle TimerHandle_UpdateCharacterVelocity;
    
    // Current target player character velocity
    float TargetVelocity;
    
    // Current velocity value change/ step (+/-)
    int32 TargetVelocityUpdateStep;
    
    /*
      Whether the sprint button/ key has been pressed while
      the sprinting functionality activation was blocked from the player 
     */
    bool bSprintNotified = false;
    
    
    /* Main functions */
    
    // Called once in the construct to load the player custom control-settings
    void LoadPlayerSettings();
    
    /*
      (save/ load game system has been cancelled for some technical reasons)
     */
	
    /* function won't be called anywhere */
    // Called once in begin play func to load player's properties
    void LoadPlayerProperties();
    
    /* function won't be called anywhere */
    /*
      Called once in begin play func to load player's power stones
      and connect them with the same power stones actors in the world
     */
    void LoadPowerStones(const class UMySaveGame* SaveGameInstance);
    
    
    /******************************************/
    
    
    /*
      Called to start attacking once the player hit once/ holds down the attack button/ key
      this method activates single attack func 
     */
    void StartAttack();
    
    // Called for a single attack
    void SingleAttack() const;
    
    /*
      Called for a single attack using right hand weapon (mouse)
      or a single attack using left hand weapon (gamepad) 
      when the player has a dual weapons
     */
    void AdsSingleAttack() const;
    
    // Called for right hand weapon attack
    void RightHandWeaponAttack(const bool IsAdsAttack);
    
    // Called for left hand weapon attack
    void LeftHandWeaponAttack(const bool IsAdsAttack);
    
    // Called to stop attacking with the right hand weapon
    void StopRightHandAttack();
    
    // Called to stop attacking with the left hand weapon
    void StopLeftHandAttack();
    
    // Called to check if the player is near a wall (mesh) - to prevent the weapons from going in a wall
    void CheckIfNearWall();
    
    
    /* Using weapons */
    
    /*
      Called when the player hit once the button (gamepad face button up)/ mouse scroll wheel up
      to swap to the next weapon slot (or back to slot 1 - or none)
     */
    void UseNextWeapon();
    
    /*
      Called when the player hit once the button mouse scroll wheel down
      to swap to the previous weapon slot (or back in reverse to slot 4 - or none)
     */
    void UsePreviousWeapon();
    
    
    /* Player interaction methods */
    
    /*
      Called when the player hit once the button/ key to interact
      with an interactable actors across the world
     */
    void Interact();

	// Called when the player release the interact button/ key
	void ReleaseInteract();
    
    // Called when the player holds down the button/ key to interact with an item actor in the world
    void InteractItem();
    
    // Called when the player holds down the button/ key to interact with a door actor in the world
    void InteractDoor(class ARegularDoor* InDoor);
    
    // Called when the player holds down the button/ key to interact with a weapon actor in the world
    void InteractWeapon();
    
    // Called if interact weapon succeeded to find space for a new weapon
    void SuccessInteractWeapon();
    
    // Called when the player holds down the button/ key to interact with a mystery box actor in the world
    void InteractMysteryBox(class AMysteryBoxBase* InMysteryBox);
    
    // Called when the player holds down the button/ key to interact with an ammo item actor in the world
    void InteractAmmo();
    
    // Called when the player holds down the button/ key to interact with a wall weapon buy actor in the world
    void InteractWallWeapon(class AWallWeaponBuy* InWallWeaponBuy);
    
    // Called when the player holds down the button/ key to interact with a power stone actor in the world
    void InteractPowerStone(APowerStone* InPowerStone);

	// Called when the player holds down the button/ key to interact with a quest item actor in the world
	void InteractQuest(class AQuestItem* InQuestItem);

	// Called when the player holds down the button/ key to interact with a quest items crafting table actor in the world
	void InteractQuestItemsCraftingTable(class AQuestItemsCraftingTable* InQuestItemsCraftingTable) const;
    
    // Called when the player holds down the button/ key to interact with a cash actor in the world
    void InteractCashActor(class ACashActor* InCashActor);

	// Called when the player hold down the button/ key to interact with an obstacle actor in the world
	void InteractObstacle(class AObstacle* InObstacleActor);

	// Called when the player holds down the button/ key to interact with a double souls power stone in the world
	void InteractDoubleSoulsPowerStone(APowerStone* InPowerStone);

	// Called when the player holds down the button/ key to interact with a rapid fire power stone in the world
	void InteractRapidFirePowerStone(APowerStone* InPowerStone);

	// Called when the player holds down the button/ key to interact with a big shoulders power stone in the world
	void InteractBigShouldersPowerStone(APowerStone* InPowerStone);

	// Called when the player holds down the button/ key to interact with a speedy power stone in the world
	void InteractSpeedyPowerStone(APowerStone* InPowerStone);
    
    
    /******************************************/
    
    
    /* Footsteps */
    
    // Called to check the player's velocity for playing foot steps sounds and effects
    void CheckVelocityForFootsteps();
    
    // Whether interacting with an ammo item happened by buying ammo of a wall weapon
    bool IsWallWeaponState = false;
    
    // Current running interaction functionality wall weapon cost
    int32 CurrentWallWeaponCost = 0;
    
    // Current running interaction functionality mystery box cost
    int32 CurrentMysteryBoxCost = 0;
    
    /*
      Called to look for an interactable actor is in a line sight
      of the player's character, to update the actors interaction widgets
      like showing them or hiding them from the viewport
     */
    UFUNCTION()
    void LookForInteractableActorInteractionWidget();

	// Called to update mystery box interaction widget if in sight of the player
	void CheckMysteryBoxInteractionWidget(AMysteryBoxBase* InMysteryBox, class UInteractionWidget* Widget) const;

	// Called to update wall weapon interaction widget if in sight of the player
	void CheckWallWeaponBuyInteractionWidget(AWallWeaponBuy* InWallWeaponBuy, UInteractionWidget* Widget) const;

	// Called to update ammo item interaction widget if in sight of the player
	void CheckAmmoItemInteractionWidget(AAmmoItem* InAmmoItem, UInteractionWidget* Widget) const;

	// Called to update weapon item interaction widget if in sight of the player
	static void CheckWeaponItemInteractionWidget(AWeapon* InWeapon, UInteractionWidget* Widget);

	// Called to update quest item interaction widget if in sight of the player
	static void CheckQuestItemInteractionWidget(AQuestItem* InQuestItem, UInteractionWidget* Widget);
	
	// Called to update quest items crafting table actor's interaction widget if in sight of the player
	void CheckQuestItemsCraftingTableInteractionWidget(AQuestItemsCraftingTable* InQuestItemsCraftingTable,
		UInteractionWidget* Widget) const;

	// Called to update door interaction widget if in sight of the player
	void CheckDoorInteractionWidget(ARegularDoor* InDoor, UInteractionWidget* Widget) const;

	// Called to update power stone interaction widget if in sight of the player
	void CheckPowerStoneInteractionWidget(APowerStone* InPowerStone, UInteractionWidget* Widget) const;

	// Called to update obstacle interaction widget if in sight of the player
	void CheckObstacleInteractionWidget(AObstacle* InObstacleActor, UInteractionWidget* Widget) const;
    
    // Timer handle for looking for an interactable actor every some amount of time (about 20ms)
    FTimerHandle TimerHandle_LookForInteractableActor;
    
    // Interactable actor reference - that has been/ still in sight
    UPROPERTY()
    AInteractable* ForwardInteractableActor;
    
    
    /* Sounds */

	// Called to increase the heart beats sound component volume multiplier
	UFUNCTION()
	void IncreaseHeartBeatsSfxVolumeMultiplier();
	
	// Timer handle for increasing heart beats sound component volume multiplier
	FTimerHandle TimerHandle_IncreaseHeartBeatsSfxVolMult;
	
	// Current heart beat sound component volume multiplier
	float CurrentHeartBeatVolMult = 0.0f;
	
	// Heart beat sound component target volume multiplier
	float HeartBeatVolMultTarget = 0.0f;
	

	/******************************************/
	
	
	/* Using items from inventory or weapons wheel */
	
	// Called to use a key item (quest)
	void UseQuestItem(UItemObject* InQuestItemObject);
	
	/*
	  Called to use a weapon from the inventory component widget or
	  the weapons selection wheel component widget
	 */
	void UseWeapon(UItemObject* InWeaponItemObject);
	
	
	/* Reset can attack functions */
	
	// Called to reset whether the right hand can attack or cannot
	void ResetCanRightHandAttack();
	
	// Called to reset whether the left hand can attack or cannot
	void ResetCanLeftHandAttack();
	
	
	/* Player die/ hurt functions */
	
	// Called when the player health reaches down to 0
	void PlayerDie();
	
	// Called to increase blood amounts of blood screen material
	void AddBloodScreen();
	
	// Called to update the blood screen effect amounts
	void UpdateBloodScreen();
	
	// Player death actor class
	UPROPERTY(EditDefaultsOnly, Category = "Player Death")
	TSubclassOf<class APlayerDeathActor> PlayerDeathActorClass;
	
	// Whether the player can sprint or not
	bool bCanCharacterRun = false;
	
	
	/* Parameters used for the gamepad dynamic sprint */
	
	// Last time (in seconds) the player pressed sprint key
	float LastSprintOnGamepad = 0.f;
	
	// once the sprint key pressed , value is false, otherwise (released) true
	bool IsSprintReleased = true;
	
	// Initial delay in seconds (float) -> sprint until non-sprint
	float SprintSwitchInterruption = 1.f;
	
	// To increase Delay if the sprint key pressed
	float SwitchIncreaseDelay = 0.6f;
	
	// To decrease delay everytime sprint key released
	float SwitchDecreaseDelay = 0.1f;
	
	// Forward axis value (Left stick y)
	float ForwardAxisValue = 0.f;
	
	// Counting how many time sprint key has been pressed
	float CounterSprintPressed = 0.f;
	
	/*
	  Maximum forward axis value
	  if it's equal or smaller then no matter what, just set
	  the maximum walking speed to normal (no sprinting)
	 */
	float MaximumForwardAxisValue = 0.40f;
	
	/*
	  Minimum forward axis value
	  if it's equal or bigger then stuck on sprinting
	  only if sprint key was already pressed
	 */
	float MinimumForwardAxisValue = 0.41f;
	
	
	/******************************************/
	
	
	// Player camera manager
	UPROPERTY()
	APlayerCameraManager* PlayerCameraManager = nullptr;
	
	
	/* Player character components */
	
	// Player character camera component
	UPROPERTY()
	UCameraComponent* PlayerCamera = nullptr;
	
	// Player character inventory component
	UPROPERTY()
	UInventory* Inventory = nullptr;

	// Player character quest inventory component
	UPROPERTY()
	UQuestInventory* QuestInventory = nullptr;

	// Player character quest handler component
	UPROPERTY()
	class UQuestHandler* QuestHandlerComp = nullptr;

	// Player shield component
	UPROPERTY()
	AShieldActorComponent* ShieldComp = nullptr;
	
	// Player character weapons selection wheel component
	UPROPERTY()
	UWeaponsSelectionWheel* WeaponsWheel = nullptr;
	
	// Player character skeletal mesh component
	UPROPERTY()
	USkeletalMeshComponent* PlayerMesh;
	
	// Player character aim assist component (for gamepad users)
	UPROPERTY()
	class UAimAssistComponent* AimAssistComp;
	
	// Player interaction line trace distance (3.4 meters)
	UPROPERTY(EditAnywhere, Category = "Player Abilties")
	float PlayerInteractDistance = 340.f;
	
	// Player current weapon reference
	UPROPERTY()
	AWeapon* CurrentWeapon = nullptr;
	
	// Player current left hand weapon reference
	UPROPERTY()
	AWeapon* LeftHandCurrentWeapon = nullptr;
	
	// Current item object in use reference
	UPROPERTY()
	UItemObject* CurrentItemObjectInUse = nullptr;
	
	// Whether the player can interact or not
	bool CanInteract = true;
	
	// Whether the player can reload a weapon or not
	bool CanReload = true;
	
	// Whether the player left hand can reload or not
	bool LeftHandCanReload = true;
	
	/*
	  Current weapon class reference before spawning it
	  (immediately after calling use weapon - before spawning the weapon)
	 */
	TSubclassOf<AWeapon> WeaponClassOnUse;
	
	/*
	  Current item object reference before calling to spawn the item
	  (immediately after calling use item (type) - before spawning the item)
	 */
	UPROPERTY()
	UItemObject* ItemObjectOnUse;
    
	// Current item to be picked up reference
	UPROPERTY()
	AItem* ItemToPickUp = nullptr;
	
	
	// The weapon class that will be spawned on begin play
	UPROPERTY(EditDefaultsOnly, Category = "Begin Play Weapon")
	TSubclassOf<AWeapon> BeginPlayWeaponClass;
	
	/*
	  The ammo items classes references, that could be spawned
	  in some situations during gameplay player interactions
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Ammo Items Classes")
	TMap<TEnumAsByte<EWeaponType>, TSubclassOf<AAmmoItem>> AmmoItemsClasses;
	
	
	/******************************************/
	
	
	/* Footsteps */
	
	// Timer handle for activating foot steps
	FTimerHandle TimerHandle_ActivateFootstep;
	
	// Whether the foot step did not activated or it did (begins with true - footsteps not activated)
	bool FootstepsDoOnce = true;
	
	
	/* Regain health */
	
	// Whether to regain health or not
	bool bRegainHealth = false;

	
	bool bHasShield = false;
};
