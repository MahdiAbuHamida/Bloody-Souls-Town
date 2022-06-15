// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/InteractableActors/Interactable.h"
#include "MysteryBoxBase.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API AMysteryBoxBase final : public AInteractable
{
	GENERATED_BODY()

public:
	AMysteryBoxBase();

	// Called to open the mystery box
	void OpenBox();

	// Called to activate the mystery box
	void ActivateMysteryBox();

	// Called to deactivate the mystery box
	void DeactivateMysteryBox();

	// Called to notify the mystery box to deactivates while it is opened
	void NotifyToDeactivateMysteryBoxAfterClose();

	// Called to close the mystery after the player got a weapon from it (immediate close)
	void ImmediateClose();

	
	/* Getters */

	// Method returns whether the mystery box is open or not
	bool IsMysteryBoxOpen() const;

	// Method returns whether the mystery box is active or not
	bool IsMysteryBoxActive() const;

	// Method returns the weapons inside the mystery box
	TArray<class AWeapon*> GetBoxWeaponsArray() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Scene (main) component of the mystery box's move-able "animated" parts (the sub components)
	UPROPERTY(VisibleDefaultsOnly, Category = "Actor Components")
	USceneComponent* AnimatedSceneComp;

	// Collision box component
	UPROPERTY(EditAnywhere, Category = "Actor Components")
	class UBoxComponent* CollisionBox;

	// Point light component
	UPROPERTY()
	class UPointLightComponent* PointLightComp;

	// Particle component
	UPROPERTY(EditDefaultsOnly, Category = "Actor Components")
	UParticleSystemComponent* LightShaftParticleComp;

	// Light shaft particle system template
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
	UParticleSystem* LightShaftParticle;

	// Light shaft particle relative transform
	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	FTransform LightShaftRelativeTransform;
	
	// Weapons classes to be inside the mystery box
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TArray<TSubclassOf<AWeapon>> WeaponsSelectionClasses;

	// Music to play when the mystery box opens
	UPROPERTY(EditDefaultsOnly, Category = "Sound Effects")
	USoundBase* MysteryBoxOpenMusic;

	// Speed of location curve of the top part of the mystery box (Open box)
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float OpenTopBoxLocationInterpolationSpeed = 100.0f;

	// Speed of rotation curve of the top part of the mystery box (Open box)
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float OpenTopBoxRotationInterpolationSpeed = 100.0f;

	// Speed of location curve of the top part of the mystery box (Close box)
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float CloseTopBoxLocationInterpolationSpeed = 100.0f;

	// Speed of rotation curve of the top part of the mystery box (Close box)
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float CloseTopBoxRotationInterpolationSpeed = 100.0f;

	// Speed of raising up the weapons (Open box)
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float RaiseWeaponInterpolationSpeed = 100.0f;

	// Speed of lowering down the weapons (Close box)
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float LowerWeaponsInterpolationSpeed = 100.0f;

	// Speed of location curve the point light (Open/ Close box)
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float PointLightLocInterpolationSpeed = 100.0f;

	// Speed of lower/ increase point light intensity (Open/ Close box)
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float PointLightIntensityInterpolationSpeed = 100.0f;

	// Change selected weapon rate (seconds)
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float ChangeSelectedWeaponRate = 0.3f;

	// Stop changing selected weapon delay
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float StopChangeSelectedWeaponDelay = 3.0f;

	// Start lowering the weapons delay
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float StartLowerWeaponsDelay = 6.0f;

private:
	// Called to add the particle
	void AddLightShaft();

	/*
      Method called every x second to open the Mystery Box:
       * The top part of the Mystery Box (The root component of the meshes parts) which will be raised up
	     (Z axis) and rotates by the roll axis.
       * The current selected weapon (along the time while the selected weapon will be changed) will be raised up
       * The Point light component's intensity will be increased, also it will be raised up (Z axis)
    */
	UFUNCTION()
	void OpenBoxRaiseWeapons(); 
	
	FTimerHandle TimerHandle_OpenBox;

	/*
	  Method called every x seconds which will select a new random weapon from the box,
	  By assigning it & make it visible in-game (the previous weapon will set to be hidden in-game)
     */
	UFUNCTION()
	void ChangeSelectedWeapon();
	
	FTimerHandle TimerHandle_ChangeSelectedWeapon;

	// Called to stop changing the selected weapon
	UFUNCTION()
	void StopChangeSelectedWeapon();

	FTimerHandle TimerHandle_StopChangingSelectedWeapon;

	// Called to lower the weapons
	UFUNCTION()
	void LowerWeapons(const bool InbImmediate);
	
	FTimerHandle TimerHandle_LowerWeapons;

	// Called to stop lowering the weapons
	void StopLoweringWeapon(const bool InbImmediate);

	// Called to close the mystery box
	UFUNCTION()
	void CloseBox(const bool InbImmediate);
	
	FTimerHandle TimerHandle_CloseBox;

	// Called to stop closing the mystery box
	UFUNCTION()
	void StopClosingBox();

	// Mystery box weapons array
	TArray<AWeapon*> BoxWeaponsArray;

	// The selected weapon
	UPROPERTY()
	AWeapon* SelectedWeapon;

	
	/* Properties for when the box open/close */
	
	FVector CurrentTopBoxLocation;
	FRotator CurrentTopBoxRotation;
	float BoxLocationY_Value;
	float BoxLocationZ_Value;
	float BoxRotationRoll_Value;

	
	/* Properties for when the weapon rise up or goes down after the box open/ close */
	
	FVector CurrentWeaponLocation;
	FVector BottomWeaponLocation;
	FVector TopWeaponLocation;
	
	// Current point light component location
	FVector CurrentLightLocation;
	
	// Point light component top location
	FVector TopPointLightLocation;
	
	// Bottom light component location
	FVector BottomPointLightLocation;
	
	// Current point light component intensity
	float CurrentPointLightIntensity;
	
	// Point light component max intensity
	float PointLightMaxIntensity;


	// Whether the mystery box is open or not
	bool IsOpen;
	
	// Whether the mystery box is active or not
	bool IsActive;

	// Whether the mystery box is notified to get deactivated or not
	bool NotifiedAfterCloseToDeactivate = false;
};
