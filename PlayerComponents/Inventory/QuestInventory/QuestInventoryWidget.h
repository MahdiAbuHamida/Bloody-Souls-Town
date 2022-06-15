// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/Characters/AlexCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "QuestInventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API UQuestInventoryWidget final : public UUserWidget
{
	GENERATED_BODY()

public:
	UQuestInventoryWidget(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
	{
		AAlexCharacter* PlayerCharacter = Cast<AAlexCharacter>(UGameplayStatics::GetPlayerPawn(
			this, 0));
		if (PlayerCharacter != nullptr)
		{
			this->QuestInventoryComponent = PlayerCharacter->GetPlayerQuestInventory();
		}
	}
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Refresh();

protected:
	UPROPERTY(BlueprintReadWrite)
	UQuestInventory* QuestInventoryComponent;
};
