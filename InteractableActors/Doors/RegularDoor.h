// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/InteractableActors/Interactable.h"
#include "Components/TimelineComponent.h"
#include "RegularDoor.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API ARegularDoor : public AInteractable
{
	GENERATED_BODY()
	
public:
	ARegularDoor();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	/*
	  Called to open the door in a direction based on
	  the player location and the door's location
     */
	void ToggleDoor(const AActor* OverlappedActor);

	
	/* Getters */

	// Method returns whether the door is open or not
	bool IsDoorOpen() const;

	// Method returns whether to hide the door's interaction widget after being opened or not
	bool GetHideWidgetAfterOpened() const;

	// Method returns whether to show the door's interaction widget or not
	bool IsShowWidget() const;

	// Method returns the second door (like in double doors)
	ARegularDoor* GetSecondDoor() const;

	// Method returns the second door id
	FName GetSecondDoorId() const;

	// Method returns whether to spawn an enemy spawner after opening the door or not
	bool IsEnemySpawnerAfterOpenedAllowed() const;

	/*
	  Method returns the arrow forward direction component of the enemy spawner
	  enemy spawner which could be spawned after the door opens
	 */
	class UArrowComponent* GetDoorEnemySpawnerArrowComponent() const;

	// Method returns the enemy spawner class
	UClass* GetEnemySpawnerClass() const;

	
	/* Setters */

	// Method to set whether to show the interaction widget or not
	void SetShowWidget(const bool InShowWidget);

	
	// Whether spawned an enemy spawner or not
	bool bSpawnedEnemySpawner = false;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Arrow forward direction component (determine where to spawn enemy spawner)
	UPROPERTY(EditAnywhere, Category = "Enemy Spawner")
	UArrowComponent* EnemySpawnerArrowComponent;

	// Enemy spawner class
	UPROPERTY(EditAnywhere, Category = "Enemy Spawner")
	UClass* EnemySpawnerClass;

	// Second door id
	UPROPERTY(EditAnywhere, Category = "Door Id")
	FName SecondDoorId = "Door Id - ";


	/* Booleans */

	// Whether this is a double doors or not
	UPROPERTY(EditAnywhere, Category = "Booleans")
	bool bDoubleDoors = false;

	// Whether the door can be opened or not
	UPROPERTY(EditAnywhere, Category = "Booleans")
	bool bCanDoorBeOpened = true;

	// Whether the door can be closed after been opened
	UPROPERTY(EditAnywhere, Category = "Booleans")
	bool bCanDoorBeClosedAfterOpened = true;

	// Whether to inverse door toggle rotation or not
	UPROPERTY(EditAnywhere, Category = "Booleans")
	bool Inverse = false;

	// Whether the door is automatically opens or not
	UPROPERTY(EditAnywhere, Category = "Booleans")
	bool bDoorAutomaticallyOpens = false;

	// Whether the door is automatically closes or not
	UPROPERTY(EditAnywhere, Category = "Booleans")
	bool bDoorAutomaticallyCloses = false;

	// Whether the door is automatically opens or not (by AI)
	UPROPERTY(EditAnywhere, Category = "Booleans")
	bool bCanDoorAutoOpenByAI = false;
	
	// Whether the door is automatically closes or not (by AI)
	UPROPERTY(EditAnywhere, Category = "Booleans")
	bool bCanDoorAutoCloseByAI = false;

	// Whether to hide the interaction widget after the door opens or not 
	UPROPERTY(EditAnywhere, Category = "Booleans")
	bool bHideWidgetAfterOpened = false;

	// Whether the second door can be opened or not
	UPROPERTY(EditAnywhere, Category = "Booleans")
	bool bSecondDoorCanBeOpened = true;

	// Whether to spawn an enemy AI spawner or not after the door opens 
	UPROPERTY(EditAnywhere, Category = "Booleans")
	bool bAllowToSpawnEnemySpawner = false;

	// Door open/ close sfx
	UPROPERTY(EditAnywhere, Category = "Sounds")
	TMap<FName, USoundBase*> DoorSoundsCollection;

	// Initial rotation yaw of the door static mesh component
	float InitialRotationYaw;

	// Whether the door is open or not
	bool IsOpen;

	// Whether the door is ready to open/ close or not
	bool IsReadyState;

	// Whether to show the interaction widget or not
	bool bShowWidget = true;

	// The target rotate value
	float RotateValue;

	// Curve float value
	float CurveFloatValue;

	// Timeline value
	float TimelineValue;

	// Door static mesh rotation value
	FRotator DoorRotation;

	// Rotation timeline
	FTimeline DoorTimeLine;
	
private:
	// Box collision component
	UPROPERTY(EditAnywhere, Category = "Actor Components")
	class UBoxComponent* DoorBox;

	// Audio component
	UPROPERTY()
	UAudioComponent* DoorAudioComponent;

	// Door curve float
	UPROPERTY(EditAnywhere)
	UCurveFloat* DoorCurve;

	/*
	  Called every frame of the door's rotating-linear-curve timeline
	  to update the door's rotation based on the timeline value
	 */
	UFUNCTION()
	void ControlDoor();

	// Setting the door state
	UFUNCTION()
	void SetDoorState(const bool bReadyState);

	// Called when the timeline finishes to set the door state
	UFUNCTION()
	void OnTimelineFinished(const bool bReadyState);

	// Called to try to play a sfx
	void TryPlayOpenDoorSfx();

	// Called when another actor overlaps with the door's box component's collision
	UFUNCTION()
	void OnDoorBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
	
	// Called when another actor leaves the door's box component's collision area
	UFUNCTION()
	void OnDoorBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	// Door opening sfx name
	const FName DoorOpeningSfxName = FName("DoorOpeningSfx");

	// Door closing sfx name
	const FName DoorClosingSfxName = FName("DoorClosingSfx");

	// Second door reference (double doors)
	UPROPERTY()
	ARegularDoor* SecondDoor;
};
