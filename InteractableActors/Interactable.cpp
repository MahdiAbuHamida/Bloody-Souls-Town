// Mahdi Abu Hamida - Final University Project (2022)


#include "Interactable.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the scene component and make it the root component
	this->SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	this->SetRootComponent(this->SceneRoot);

	// Create the static mesh component and attaching it to the root component
	this->StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	this->StaticMeshComponent->SetupAttachment(this->SceneRoot);

	// Create the collision sphere component and attaching it to the root component
	this->CollisionSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	this->CollisionSphereComponent->InitSphereRadius(this->SphereRadius);
	this->CollisionSphereComponent->SetCollisionProfileName("Trigger");
	this->CollisionSphereComponent->SetupAttachment(this->SceneRoot);
	this->CollisionSphereComponent->SetCollisionObjectType(ECC_GameTraceChannel3);
	this->CollisionSphereComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(this->InteractionWidgetClass)) return;

	this->InteractionWidget = Cast<UInteractionWidget>(CreateWidget
			(this->GetWorld()->GetGameInstance(), this->InteractionWidgetClass));
		
	if (this->InteractionWidget != nullptr)
	{
		this->InteractionWidget->Init(this->WidgetActionText, this->Cost, this->ObjectNameActionTextColor,
			this->ObjectName, this->AdditionalText);
	}
}

// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


/* Getters */

// Method returns the sphere collision component
USphereComponent* AInteractable::GetSphereComponent() const
{
	return this->CollisionSphereComponent;
}

// Method returns the static mesh component
UStaticMeshComponent* AInteractable::GetStaticMeshComponent() const
{
	return this->StaticMeshComponent;
}

// Method returns the interaction widget
UUserWidget* AInteractable::GetInteractionWidget() const
{
	return this->InteractionWidget;
}

// Method returns the cost (price)
int32 AInteractable::GetCost() const
{
	return this->Cost;
}

// Method returns the "action" text of the interaction widget
FText AInteractable::GetWidgetActionText() const
{
	return this->WidgetActionText;
}

FText AInteractable::GetWidgetObjectNameText() const
{
	return this->ObjectName;
}


/* Setters */

// Method to set the cost value
void AInteractable::SetCost(const int32 InCost)
{
	this->Cost = InCost;
	if (this->InteractionWidget != nullptr)
	{
		this->InteractionWidget->SetCost(this->Cost);
	}
}
