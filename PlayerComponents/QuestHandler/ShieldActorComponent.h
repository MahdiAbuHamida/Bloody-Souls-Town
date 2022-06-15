// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShieldActorComponent.generated.h"

UCLASS()
class BLOODYSOULSTOWN_API AShieldActorComponent final : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShieldActorComponent();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	
	/* Setters */
	
	void SetItemObject(class UItemObject*);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Actor Components")
	UStaticMeshComponent* ShieldMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	class USoundCue* HitShieldSfx;

private:
	bool IsShieldDestroyed() const;

	UPROPERTY()
	UItemObject* QuestItemObject;
	
	UPROPERTY(EditDefaultsOnly, Category = "Default Values")
	float MaxProtectionValue = 250.0f;

	float CurrentProtectionValue;
};
