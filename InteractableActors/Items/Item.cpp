// Mahdi Abu Hamida - Final University Project (2022)


#include "Item.h"

#include "ItemObjects/ItemObject.h"
#include "BloodySoulsTown/Other/MyGameInstance.h"
#include "Kismet/GameplayStatics.h"


AItem::AItem()
{
	this->StaticMeshComponent->SetRenderCustomDepth(this->bUseRenderCustomDepth);
	this->InteractableType = Interactable_Item;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	if (this->ItemObject == nullptr)
	{
		this->GetDefaultItemObject();
	}
	
	if (this->GetWorld() == nullptr) return;
	FString MapName = this->GetWorld()->GetMapName();
	MapName.Append("_");
	const FName NewItemId = FName(MapName.Append(this->GetName()));
	this->SetItemId(NewItemId);

	if (!this->IsInSpawningState)
	{
		this->GetWorldTimerManager().SetTimer(this->TimerHandle_CheckIfDestroyed, this,
			&AItem::CheckIfAlreadyDestroyed, 0.2f, false);
	}
}

// Called to initialize the item object
UItemObject* AItem::GetDefaultItemObject()
{
	return this->ItemObject;
}

// Called to check if the item has been already destroyed
void AItem::CheckIfAlreadyDestroyed()
{
	if (const UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GetGameInstance()))
	{
		if (MyGameInstance->CheckIfAlreadyDestroyed(this->ItemObject->GetItemId()))
		{
			this->Destroy();
		}
	}
}

// Method returns the item object
UItemObject* AItem::GetItemObject() const
{
	return this->ItemObject;
}

// Method returns the item id
FName AItem::GetItemId() const
{
	return this->ItemObject->GetItemId();
}

// Method returns whether the item is currently spawning or not
bool AItem::GetIsInSpawningState() const
{
	return this->IsInSpawningState;
}

// Method to set the item id
void AItem::SetItemId(const FName InItemId) const
{
	this->ItemObject->SetItemId(InItemId);
}

// Method to set the item object
void AItem::SetItemObject(UItemObject* InItemObject)
{
	this->ItemObject = InItemObject;
}

// Method to set whether the item is currently spawning or not
void AItem::SetIsInSpawningState(const bool InIsInSpawningState)
{
	this->IsInSpawningState = InIsInSpawningState;
}
