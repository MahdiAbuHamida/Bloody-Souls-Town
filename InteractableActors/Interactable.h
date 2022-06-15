// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"

#include "Enumurations/InteractableActorType.h"
#include "Components/SphereComponent.h"
#include "InteractionWidget.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

UCLASS()
class BLOODYSOULSTOWN_API AInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractable();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	/* Getters */

	// Method returns the sphere collision component
	USphereComponent* GetSphereComponent() const;

	// Method returns the static mesh component
	UStaticMeshComponent* GetStaticMeshComponent() const;

	// Method returns the interaction widget
	UUserWidget* GetInteractionWidget() const;

	// Method returns the cost (price)
	int32 GetCost() const;

	// Method returns the "action" text of the interaction widget
	FText GetWidgetActionText() const;

	FText GetWidgetObjectNameText() const;

	
	/* Setters */

	// Method to set the cost value
	void SetCost(const int32 InCost);

	// Type of the interactable actor
	EInteractableActorType InteractableType;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	/* Actor default components */
	
	// Scene (main) component of this actor
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* SceneRoot;

	// Static mesh component of this actor
	UPROPERTY(EditAnywhere, Category = "Actor Components")
	UStaticMeshComponent* StaticMeshComponent;

	// Collision sphere component of this actor
	UPROPERTY(VisibleAnywhere, Category = "Actor Components")
	USphereComponent* CollisionSphereComponent;

	// Radius of the sphere collision
	UPROPERTY(EditAnywhere)
	float SphereRadius = 50.0f;

	
	/* UserInterface related */
	
	UPROPERTY(EditAnywhere, Category = "User Interface")
	TSubclassOf<UUserWidget> InteractionWidgetClass;
	
	UPROPERTY()
	UInteractionWidget* InteractionWidget;
	
	UPROPERTY(EditAnywhere, Category = "User Interface")
	FText WidgetActionText;

	UPROPERTY(EditAnywhere, Category = "User Interface")
	FLinearColor ObjectNameActionTextColor = FLinearColor(0,0,0, 0);

	UPROPERTY(EditAnywhere, Category = "User Interface")
	FText ObjectName = FText::FromString("Object Name");

	UPROPERTY(EditAnywhere, Category = "User Interface")
	FText AdditionalText = FText::FromString("Additional Text");
	
	UPROPERTY(EditAnywhere, Category = "Player Interaction")
	int32 Cost = 500;
};