// Mahdi Abu Hamida - Final University Project (2022)


#include "MainGameState.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/InputSettings.h"
#include "MainGameMode.h"
#include "MainPlayerState.h"
#include "MyGameInstance.h"
#include "BloodySoulsTown/PlayerComponents/Inventory/Inventory.h"
#include "BloodySoulsTown/Characters/AlexCharacter.h"
#include "Sound/SoundCue.h"
#include "BloodySoulsTown/Characters/AI/EnemyCharacter.h"
#include "BloodySoulsTown/InteractableActors/Items/AmmoItem/AmmoItem.h"
#include "BloodySoulsTown/InteractableActors/Items/ItemObjects/AmmoItemObject.h"
#include "BloodySoulsTown/InteractableActors/WallWeaponBuy/WallWeaponBuy.h"
#include "BloodySoulsTown/InteractableActors/MysteryBox/MysteryBoxBase.h"

// Called when the game starts
void AMainGameState::BeginPlay()
{
	Super::BeginPlay();

	this->MainGameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this));
	this->PlayerCharacter = Cast<AAlexCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	
	if (this->PlayerCharacter != nullptr)
	{
		this->PlayerState = Cast<AMainPlayerState>(this->PlayerCharacter->GetPlayerState());
	}

	const UInputSettings* InputSettings = UInputSettings::GetInputSettings();
	TArray<FInputActionKeyMapping> ActionKeyMappings;
	InputSettings->GetActionMappingByName(FName("Interact"), ActionKeyMappings);
	for (FInputActionKeyMapping ActionKeyMapping : ActionKeyMappings)
	{
		if (!ActionKeyMapping.Key.IsGamepadKey())
		{
			this->InteractKeyText = ActionKeyMapping.Key.GetDisplayName();
			break;
		}
	}
	
	TArray<AActor*> MysteryBoxesFound;
	UGameplayStatics::GetAllActorsOfClass(this, AMysteryBoxBase::StaticClass(), MysteryBoxesFound);

	for (AActor* Actor : MysteryBoxesFound)
	{
		if (AMysteryBoxBase* CurrentMysteryBox = Cast<AMysteryBoxBase>(Actor))
		{
			this->MysteryBoxes.AddUnique(CurrentMysteryBox);
		}
	}

	this->GetWorldTimerManager().SetTimer(this->TimerHandle_ActivateRandomMysteryBox, this,
		&AMainGameState::ActivateRandomMysteryBox, this->DelayToActivateMysteryBox, false);
}

// Called to check whether the round ended or not
void AMainGameState::CheckIfRoundIsOver() const
{
	if (this->NumberOfDeadEnemiesCurrentRound == this->NumberOfEnemiesToSpawn && this->MainGameMode != nullptr)
	{
		this->MainGameMode->BreakBetweenRounds();
	}
}

// Called to activate a mystery box
void AMainGameState::ActivateRandomMysteryBox()
{
	this->GetWorldTimerManager().ClearTimer(this->TimerHandle_ActivateRandomMysteryBox);
	
	const int32 RandomIndex = FMath::RandRange(0, this->MysteryBoxes.Num() - 1);
	if (RandomIndex >= 0 && this->MysteryBoxes[RandomIndex] != nullptr
		&& !this->MysteryBoxes[RandomIndex]->IsMysteryBoxActive() && !this->MysteryBoxes[RandomIndex]->IsMysteryBoxOpen())
	{
		this->MysteryBoxes[RandomIndex]->ActivateMysteryBox();
		this->CurrentActivatedMysteryBoxIndex = RandomIndex;
	}

	this->GetWorldTimerManager().SetTimer(this->TimerHandle_DeactivateMysteryBox, this,
		&AMainGameState::DeactivateMysteryBox, this->DelayToDeactivateMysteryBox, false);
}

// Called to deactivate a mystery box
void AMainGameState::DeactivateMysteryBox()
{
	if (AMysteryBoxBase* MysteryBox = this->MysteryBoxes[this->CurrentActivatedMysteryBoxIndex])
	{
		if (MysteryBox->IsMysteryBoxActive())
		{
			if (MysteryBox->IsMysteryBoxOpen())
			{
				MysteryBox->NotifyToDeactivateMysteryBoxAfterClose();
			}
			else
			{
				this->MysteryBoxes[this->CurrentActivatedMysteryBoxIndex]->DeactivateMysteryBox();
				this->CurrentActivatedMysteryBoxIndex = -1;
				
				this->GetWorldTimerManager().ClearTimer(this->TimerHandle_DeactivateMysteryBox);
		
				this->GetWorldTimerManager().SetTimer(this->TimerHandle_ActivateRandomMysteryBox, this,
				&AMainGameState::ActivateRandomMysteryBox, this->DelayToActivateMysteryBox, false);
			}
		}
	}
}

// Called to increase the round
void AMainGameState::IncreaseRound()
{
	this->NumberOfDeadEnemiesCurrentRound = 0;
	this->NumberOfSpawnedEnemies = 0;
	
	++this->CurrentRound;

	if (this->PlayerState != nullptr)
	{
		this->PlayerState->IncreaseRounds();
	}
	
	if (this->CheckIfWolfsRound(this->CurrentRound))
	{
		this->NumberOfEnemiesToSpawn = FMath::Min(this->CurrentRound * 1.5f, 18.0f);
		this->bWolfsRound = true;
	}
	else
	{
		this->NumberOfEnemiesToSpawn = FMath::Min(this->CurrentRound * 2.8f + 2.2f, 150.f);	
	}

	this->OnRoundIncreasedDel.Broadcast(this->CurrentRound, this->NumberOfEnemiesToSpawn);
}

// Called when an enemy AI dies
void AMainGameState::EnemyDied(const bool InIsBoss)
{
	if (!InIsBoss)
	{
		++this->NumberOfDeadEnemiesCurrentRound;
	}
	--this->NumberOfAliveEnemies;
	++this->NumberOfAllDeadEnemies;
	++this->CurrentNumberOfDeadEnemiesBodies;
	
	int32 NumberOfEnemiesToDestroy = this->CurrentNumberOfDeadEnemiesBodies - this->LimitOfEnemiesNumberToBeginDestroy + 1;
	while (NumberOfEnemiesToDestroy > 0 && !this->EnemiesSpawnedQueue.IsEmpty())
	{
		--NumberOfEnemiesToDestroy;
		AActor* EnemyToDestroy;
		this->EnemiesSpawnedQueue.Peek(EnemyToDestroy);
		if (const AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(EnemyToDestroy))
		{
			if (EnemyCharacter->IsDead())
			{
				this->EnemiesSpawnedQueue.Pop();
				EnemyToDestroy->Destroy();
				--this->CurrentNumberOfDeadEnemiesBodies;
			}
		}
	}
	
	this->CheckIfRoundIsOver();
}

// Called to spawn an ammo item
void AMainGameState::SpawnAmmoItem(const bool IsHeadshot, const FVector Location, const FVector ForwardVector)
{
	if (this->PlayerCharacter == nullptr) return;
	
	const int8 MaximumRange = IsHeadshot ? 7 : 8;
	const int8 RandomNumber_1 = FMath::RandRange(1 ,MaximumRange), RandomNumber_2 = FMath::RandRange(1 ,MaximumRange);
	
	if (RandomNumber_1 == RandomNumber_2)
	{
		UInventory* PlayerInventory = this->PlayerCharacter->GetPlayerInventory();
		TArray<EWeaponType> AmmoTypesInv;
		if (PlayerInventory != nullptr)
		{
			TArray<UAmmoItemObject*> AmmoItems = PlayerInventory->GetAmmoItemObjects();
			for (UAmmoItemObject* AmmoItemObject : AmmoItems)
			{
				if (AmmoItemObject != nullptr)
				{
					AmmoTypesInv.AddUnique(AmmoItemObject->AmmoTypeByWeapon);
				}
			}
		}
		EWeaponType AmmoType = this->AmmoTypes[FMath::RandRange(0, this->AmmoTypes.Num() - 1)];
		if (FMath::RandBool() && AmmoTypesInv.Num() > 0)
		{
			AmmoType = AmmoTypesInv[FMath::RandRange(0, AmmoTypesInv.Num() - 1)];
		}
		else if (FMath::RandBool() && this->PlayerCharacter->GetCurrentWeapon() != nullptr)
		{
			if (this->PlayerCharacter->GetCurrentWeapon()->GetWeaponType() != EWeaponType::Weapon_Melee)
			{
				AmmoType = this->PlayerCharacter->GetCurrentWeapon()->GetWeaponType();
			}
		}
		if (this->PlayerCharacter->GetAmmoItemsClasses().Contains(AmmoType))
		{
			FVector SpawnLocation = Location + (ForwardVector * 150);
			FHitResult Hit;
			const FVector EndLocation = SpawnLocation / FVector(1, 1, 1000);
			if (GetWorld()->LineTraceSingleByChannel(OUT Hit,SpawnLocation,EndLocation,
				ECollisionChannel::ECC_Visibility))
			{
				SpawnLocation = Hit.Location;
			}
			if (AActor* SpawnedAmmo = this->GetWorld()->SpawnActor(this->PlayerCharacter->GetAmmoItemsClasses()[AmmoType],
				&SpawnLocation))
			{
				this->SpawnedAmmoItemsQueue.Enqueue(SpawnedAmmo);
				++this->NumberOfSpawnedAmmoItems;
				if (this->NumberOfSpawnedAmmoItems >= this->LimitOfAmmoItemsNumberToBeginDestroy)
				{
					AActor* AmmoItemToDestroy;
					this->SpawnedAmmoItemsQueue.Peek(AmmoItemToDestroy);
					if (AmmoItemToDestroy != nullptr)
					{
						AmmoItemToDestroy->Destroy();
					}
				}
			}
		}
	}
}

// Called to increase player points after damaging an AI enemy character
void AMainGameState::IncreasePlayerPoints(const float DamageScale, const bool IsEnemyDead, const FVector Location,
	const FVector ForwardVector, const bool InIsBoss)
{
	if (this->PlayerCharacter == nullptr || this->PlayerState == nullptr) return;

	const bool bHeadShot = DamageScale == 1.0f;
	const uint8 BossMult = InIsBoss ? 2 : 1;
	
	int32 PointsToIncrease;
	if (IsEnemyDead)
	{
		if (PlayerCharacter->GetLaughingSoundCue() != nullptr && PlayerCharacter->CurrentHealth >= 60
			&& FMath::RandRange(0, 10) <= 3 && !GetWorldTimerManager().IsTimerActive(LaughingActive))
		{
			UGameplayStatics::PlaySound2D(this, PlayerCharacter->GetLaughingSoundCue(),
				1.f, 1.f, 0.f, nullptr,
				PlayerCharacter, false);
			GetWorldTimerManager().SetTimer(LaughingActive, this, &AMainGameState::ClearLaughingActive,
				PlayerCharacter->GetLaughingSoundCue()->GetDuration(), false);
		}
		
		this->PlayerState->IncreaseKills();
		if (bHeadShot)
		{
			PointsToIncrease = 100 * BossMult;
			this->PlayerState->IncreaseHeadshotKills();
		}
		else
		{
			PointsToIncrease = 50 * BossMult;
		}
		this->PlayerCharacter->IncreasePlayerPoints(PointsToIncrease, false);
		this->SpawnAmmoItem(bHeadShot, Location, ForwardVector);
	}
	else
	{
		bHeadShot ? PointsToIncrease = 15 * BossMult : PointsToIncrease = 10 * BossMult;
		this->PlayerCharacter->IncreasePlayerPoints(PointsToIncrease, false);
	}
}

// Called to update mystery boxes/ wall weapons buy when the player change/ release weapon
void AMainGameState::UpdateMysteryBoxes_WallWeaponsBuyInteractionWidgets(const UClass* WeaponClass, const float MaxAmmoForGun, const float AmmoLack) const
{
	this->UpdateMysteryBoxesInteractionWidgets(WeaponClass);
	this->UpdateWallWeaponsBuyInteractionWidgets(WeaponClass, MaxAmmoForGun, AmmoLack);
}

// Called to update mystery boxes when the player change/ release weapon
void AMainGameState::UpdateMysteryBoxesInteractionWidgets(const UClass* WeaponClass) const
{
	FString MB_NewActionText("Swap With ");
	TArray<AActor*> MysteryBoxes_Local;
	UGameplayStatics::GetAllActorsOfClass(this, AMysteryBoxBase::StaticClass(), MysteryBoxes_Local);

	for (AActor* Actor : MysteryBoxes_Local)
	{
		if (const AMysteryBoxBase* MysteryBox = Cast<AMysteryBoxBase>(Actor))
		{
			TArray<AWeapon*> MysteryBoxWeaponsClasses = MysteryBox->GetBoxWeaponsArray();
			for (AWeapon* Weapon : MysteryBoxWeaponsClasses)
			{
				if (Weapon != nullptr)
				{
					if (UInteractionWidget* InteractionWidget = Cast<UInteractionWidget>(Weapon->GetInteractionWidget()))
					{
						if (Weapon->GetClass() == WeaponClass)
						{
							MB_NewActionText.Append(Weapon->GetItemObject()->GetItemName());
							InteractionWidget->SetActionText(FText::FromString(MB_NewActionText));
						}
						else
						{
							InteractionWidget->SetActionText(Weapon->SavedActionText);
						}
					}
				}
			}
		}
	}
}

// Called to update wall weapons buy when the player change/ release weapon
void AMainGameState::UpdateWallWeaponsBuyInteractionWidgets(const UClass* WeaponClass, const float MaxAmmoForGun, const float AmmoLack) const
{
	FString WwB_NewActionText("Buy ammo for ");
	TArray<AActor*> WallWeaponsBuy;
	UGameplayStatics::GetAllActorsOfClass(this, AWallWeaponBuy::StaticClass(), WallWeaponsBuy);

	for (AActor* Actor : WallWeaponsBuy)
	{
		if (AWallWeaponBuy* WallWeaponBuy = Cast<AWallWeaponBuy>(Actor))
		{
			TSubclassOf<AWeapon> WallWeaponBuyClass = WallWeaponBuy->GetWeaponClass();
			if (WallWeaponBuyClass == WeaponClass)
			{
				if (UInteractionWidget* InteractionWidget = Cast<UInteractionWidget>(WallWeaponBuy->GetInteractionWidget()))
				{
					FString WallWeaponBuyActionText = WallWeaponBuy->GetWidgetActionText().ToString();
					/*
					  This will remove the "Buy " from the interaction widget action text
					  Example: "Buy Pistol", after removing the first 4 characters it will be "Pistol"
					 */
					WallWeaponBuyActionText.RemoveAt(0, 4);
					WwB_NewActionText.Append(WallWeaponBuyActionText);
					InteractionWidget->SetActionText(FText::FromString(WwB_NewActionText));
					const float AmmoIncreaseCost = WallWeaponBuy->GetAmmoIncreaseForSameWeaponCost();
					const int32 NewCost = FMath::Max((AmmoLack / MaxAmmoForGun) * AmmoIncreaseCost, 0.f);
					WallWeaponBuy->SetCost(NewCost);
				}
			}
			else if (UInteractionWidget* InteractionWidget = Cast<UInteractionWidget>(WallWeaponBuy->GetInteractionWidget()))
			{
				InteractionWidget->SetActionText(WallWeaponBuy->ActionTextCopy);
				WallWeaponBuy->SetCost(WallWeaponBuy->CostCopy);
			}
		}
	}
}

// Called to check whether a round is a wolfs round or not and returns the result
bool AMainGameState::CheckIfWolfsRound(const int32 Round)
{
	if (Round != 0 && Round % 10 % 5 == 0)
	{
		return Round > 10 && Round % 10 == 0 || Round <= 10;
	}
	return false;
}

// Called to set the gamepad right stick dead zone
void AMainGameState::SetGamepadRightStickDeadZone(const float Value)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController == nullptr) return;

	FInputAxisProperties RightX_AxisProperties, RightY_AxisProperties;
	PlayerController->PlayerInput->GetAxisProperties(FKey(FName("Gamepad_RightX")), RightX_AxisProperties);
	PlayerController->PlayerInput->GetAxisProperties(FKey(FName("Gamepad_RightY")), RightY_AxisProperties);

	RightX_AxisProperties.DeadZone = Value;
	RightY_AxisProperties.DeadZone = Value;

	PlayerController->PlayerInput->SetAxisProperties(FKey(FName("Gamepad_RightX")), RightX_AxisProperties);
	PlayerController->PlayerInput->SetAxisProperties(FKey(FName("Gamepad_RightY")), RightY_AxisProperties);
}

// Called to set the gamepad left stick dead zone
void AMainGameState::SetGamepadLeftStickDeadZone(const float Value)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController == nullptr) return;

	FInputAxisProperties LeftX_AxisProperties, LeftY_AxisProperties;
	PlayerController->PlayerInput->GetAxisProperties(FKey(FName("Gamepad_LeftX")), LeftX_AxisProperties);
	PlayerController->PlayerInput->GetAxisProperties(FKey(FName("Gamepad_LeftY")), LeftY_AxisProperties);

	LeftX_AxisProperties.DeadZone = Value;
	LeftY_AxisProperties.DeadZone = Value;

	PlayerController->PlayerInput->SetAxisProperties(FKey(FName("Gamepad_LeftX")), LeftX_AxisProperties);
	PlayerController->PlayerInput->SetAxisProperties(FKey(FName("Gamepad_LeftY")), LeftY_AxisProperties);
}

// Called to spawn an item out of an actor
void AMainGameState::SpawnItemFromActor(UItemObject* InItemObject, AActor* InActor, bool GroundClamp)
{
	if (InActor == nullptr || InItemObject == nullptr) return;
	
	const FVector InActorLocation = InActor->GetActorLocation();
	const FVector InActorForwardVector = InActor->GetActorForwardVector();

	FVector SpawnLocation = InActorLocation + InActorForwardVector * 150;
	
	if (GroundClamp)
	{
		FHitResult Hit;
		const FVector EndLocation = SpawnLocation / FVector(1, 1, 1000);

		if (GetWorld()->LineTraceSingleByChannel(OUT Hit,SpawnLocation,EndLocation,
			ECollisionChannel::ECC_Visibility))
		{
			SpawnLocation = Hit.Location;
		}
	}
	
	AItem* SpawnedItem = GetWorld()->SpawnActor<AItem>(InItemObject->GetItemClass(), SpawnLocation,
		FRotator(0, 0, 0));
	SpawnedItem->SetItemObject(InItemObject);
	SpawnedItem->SetItemId(InItemObject->GetItemId());

	if (InItemObject->GetItemType() == EItemType::Item_Ammo)
	{
		if (this->PlayerCharacter != nullptr)
		{
			UInventory* PlayerInventory = PlayerCharacter->GetPlayerInventory();
			if (PlayerInventory != nullptr)
			{
				PlayerInventory->UpdateAmmoAmounts();
			}
		}
	}
	
	TArray<AActor*> OverlappedItems;
	SpawnedItem->GetOverlappingActors(OverlappedItems, AItem::StaticClass());
	if (OverlappedItems.Num() > 0)
	{
		SpawnedItem->SetActorLocation(
			FVector(SpawnLocation.X + (OverlappedItems.Num() * 40), SpawnLocation.Y + (OverlappedItems.Num() * 40), SpawnLocation.Z)
		);
	}
	
	UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GetGameInstance());
	if (MyGameInstance->DestroyedActors.Find(InItemObject->GetItemId()) > -1)
	{
		MyGameInstance->DestroyedActors.Remove(SpawnedItem->GetItemId());
	}
}
