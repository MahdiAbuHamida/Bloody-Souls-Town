// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "BloodySoulsTown/InteractableActors/Items/QuestItem/Structs/QuestClass_Name.h"
#include "BloodySoulsTown/InteractableActors/Items/QuestItem/Structs/QuestData.h"
#include "Components/ActorComponent.h"
#include "QuestHandler.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLOODYSOULSTOWN_API UQuestHandler final : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UQuestHandler();

	FQuestClass_Name HandleShieldQuest() const;

	FQuestClass_Name HandleKeyQuest();


	/* Getters */
	FQuestData& GetShieldQuestStruct();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TMap<int32, TSubclassOf<AQuestItem>> KeyItemsClassesPerQuestId;

	UPROPERTY(EditDefaultsOnly)
	TArray<FQuestData> QuestStructsCollection;

	UPROPERTY(EditDefaultsOnly)
	TMap<TSubclassOf<AQuestItem>, FQuestClass_Name> QuestClass_Name_StructsPerClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AQuestItem> ShieldItemClass;
	
	UPROPERTY(EditDefaultsOnly)
	FQuestData ShieldQuestStruct; 

private:
	UPROPERTY()
	class UQuestInventory* QuestInventoryComp;
};
