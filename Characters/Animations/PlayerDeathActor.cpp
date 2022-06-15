// Mahdi Abu Hamida - Final University Project (2022)


#include "PlayerDeathActor.h"
#include "Components/ArrowComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/PostProcessComponent.h"

// Sets default values
APlayerDeathActor::APlayerDeathActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Creating cube static mesh component
	this->CubeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	this->SetRootComponent(this->CubeMeshComponent);

	// Setting up physics to simulate "death player scene" when this cube spawn on the player character
	this->CubeMeshComponent->SetSimulatePhysics(true);
	this->CubeMeshComponent->SetLinearDamping(3.f);
	this->CubeMeshComponent->SetAngularDamping(0.5f);
	this->CubeMeshComponent->SetEnableGravity(true);

	this->ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	this->LeftRadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("LeftForce"));
	this->LeftRadialForceComponent->SetupAttachment(this->RootComponent);
	this->LeftRadialForceComponent->Radius = 50.f;
	this->LeftRadialForceComponent->SetRelativeLocation(FVector(45.f, -30.f, 5.f));
	this->LeftRadialForceComponent->ImpulseStrength = 1000.f;
	this->LeftRadialForceComponent->bImpulseVelChange = true;

	this->RightRadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RightForce"));
	this->RightRadialForceComponent->SetupAttachment(this->RootComponent);
	this->RightRadialForceComponent->Radius = 50.f;
	this->RightRadialForceComponent->SetRelativeLocation(FVector(45.f, 30.f, 5.f));
	this->RightRadialForceComponent->ImpulseStrength = 1000.f;
	this->RightRadialForceComponent->bImpulseVelChange = true;

	// Setting post process component
	this->PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	this->PostProcessComponent->SetupAttachment(this->RootComponent);
}

// Initialize before running the constriction script
void APlayerDeathActor::Init(const FPostProcessSettings& InPostProcessSettings)
{
	// Copying previous player camera post processing settings
	this->PostProcessSettings = InPostProcessSettings;
}

// Called when the game starts or when spawned
void APlayerDeathActor::BeginPlay()
{
	Super::BeginPlay();

	this->PostProcessComponent->Settings = this->PostProcessSettings;
	const float RandomFloatForVec = FMath::FRandRange(-10.f, 10.f);
	const FVector LocAndImpulse = FVector(RandomFloatForVec, RandomFloatForVec, 0.f) * 650.f;
	this->CubeMeshComponent->AddImpulseAtLocation(LocAndImpulse, LocAndImpulse);

	// Random force from left angle or right (true = right , false = left)
	if (FMath::RandBool())
	{
		this->RightRadialForceComponent->FireImpulse();
	}
	else
	{
		this->LeftRadialForceComponent->FireImpulse();
	}
}

