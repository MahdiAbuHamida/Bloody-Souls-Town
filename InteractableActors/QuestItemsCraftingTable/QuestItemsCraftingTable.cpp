// Mahdi Abu Hamida - Final University Project (2022)


#include "BloodySoulsTown/InteractableActors/QuestItemsCraftingTable/QuestItemsCraftingTable.h"

#include "QuestItemsCraftingTableWidget.h"
#include "BloodySoulsTown/InteractableActors/Items/QuestItem/QuestItem.h"
#include "BloodySoulsTown/InteractableActors/Items/QuestItem/Structs/QuestClass_Name.h"
#include "Components/BoxComponent.h"

AQuestItemsCraftingTable::AQuestItemsCraftingTable()
{
	this->InteractableType = Interactable_PartsBuildingTable;
	
	bInUse = false;
	bCanUse = true;

	this->BoxCollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	this->BoxCollisionComp->SetupAttachment(this->GetRootComponent());

	this->BoxCollisionComp->SetCollisionObjectType(ECC_GameTraceChannel3);
}

void AQuestItemsCraftingTable::BeginPlay()
{
	Super::BeginPlay();

	if (UUserWidget* Widget = CreateWidget(GetWorld()->GetFirstPlayerController(),
		CraftingTableWidgetClass))
	{
		CraftingTableWidget = Cast<UQuestItemsCraftingTableWidget>(Widget);
	}
}

void AQuestItemsCraftingTable::UseTable(const FQuestClass_Name& InQuestClass_Name)
{
	const FString& QuestItemName = InQuestClass_Name.QuestItemName;
	
	if (!IsValid(InQuestClass_Name.QuestItemClass) || InQuestClass_Name.QuestItemName == "" || bInUse || !bCanUse
		|| CraftingTableWidget == nullptr) return;
	
	bInUse = true;
	bCanUse = false;

	if (InteractionWidget != nullptr && InteractionWidget->IsInViewport())
	{
		InteractionWidget->RemoveFromViewport();
	}
	
	FTimerManager& TimerManager = GetWorldTimerManager();
	
	if (TimerManager.IsTimerActive(TimerHandle_FinishCraftingItem))
	{
		TimerManager.ClearTimer(TimerHandle_FinishCraftingItem);
	}
	
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("FinishBuildingItem"), InQuestClass_Name.QuestItemClass);
	
	CraftingTableWidget->CustomInit(&TimerManager, QuestItemName, QuestItemsCraftingTableType, CraftItemTime);
	
	TimerManager.SetTimer(TimerHandle_FinishCraftingItem, TimerDelegate,
		CraftItemTime, false);
	
	CraftingTableWidget->AddToViewport();
}

void AQuestItemsCraftingTable::ReleaseUsingTable()
{
	FTimerManager& TimerManager = GetWorldTimerManager();
	if (TimerManager.IsTimerActive(TimerHandle_FinishCraftingItem))
	{
		TimerManager.ClearTimer(TimerHandle_FinishCraftingItem);
	}
	if (CraftingTableWidget != nullptr)
	{
		CraftingTableWidget->StopProgressTimer();
		CraftingTableWidget->RemoveFromViewport();
	}
	bInUse = false;
	bCanUse = true;
}

UBoxComponent* AQuestItemsCraftingTable::GetBoxCollisionComponent() const
{
	return this->BoxCollisionComp;
}

EQuestItemsCraftingTableType AQuestItemsCraftingTable::GetQuestItemsCraftingTableType() const
{
	return this->QuestItemsCraftingTableType;
}

bool AQuestItemsCraftingTable::GetIsInUse() const
{
	return bInUse;
}

void AQuestItemsCraftingTable::SetCanUseTable(const bool InbCanUse)
{
	this->bCanUse = InbCanUse;
}

void AQuestItemsCraftingTable::FinishBuildingItem(const TSubclassOf<AQuestItem> InQuestItemClass)
{
	ReleaseUsingTable();

	if (!IsValid(InQuestItemClass)) return;
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FVector Location = GetActorLocation() + SpawnQuestItemRelativeLocation;
	const FRotator Rotation = GetActorRotation() + SpawnQuestItemRelativeRotation;
	
	if (AQuestItem* QuestItem = Cast<AQuestItem>(this->GetWorld()->SpawnActor(InQuestItemClass, &Location, &Rotation,
		SpawnParameters)))
	{
		bCanUse = false;
		QuestItem->SetTable(this);
		if (BoxCollisionComp != nullptr)
		{
			BoxCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}
