// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionTextChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjectNameTextChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCostChanged);

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API UInteractionWidget final : public UUserWidget
{
	GENERATED_BODY()

public:
	// Custom Initialize method
	void Init(const FText InActionText, const int32 InCost, const FLinearColor InObjectNameColor, const FText InObjectName
		, const FText InAdditionalText)
	{
		this->ActionText = InActionText;
		this->Cost = InCost;
		this->ObjectNameColor = InObjectNameColor;
		this->ObjectNameText = InObjectName;
		this->AdditionalText = InAdditionalText;
		SetObjectNameText_AdditionalText_TextColor();
	}


	/* Getters */
	
	FString GetActionText() const
	{
		return ActionText.ToString();		
	}

	
	/* Setters */

	// Method to set the action text
	void SetActionText(const FText InActionText)
	{
		this->ActionText = InActionText;
		this->OnActionTextChangedDel.Broadcast();
	}

	void SetObjectNameText(const FText InObjectName)
	{
		this->ObjectNameText = InObjectName;
		this->OnObjectNameTextChanged.Broadcast();
	}

	/*
	  Method receives whether to set action text with an active color or inactive color
	  and then it sets the color
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void SetActionTextColor(const bool bActiveColor);

	UFUNCTION(BlueprintImplementableEvent)
	void SetObjectNameText_AdditionalText_TextColor();

	/*
	  Method receives whether to set cost text with an "available" color or not color
	  and then it sets the color
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void SetCostTextColor(const bool bHasEnoughPoints);

	// Method to set the cost
	void SetCost(const int32 InCost)
	{
		this->Cost = InCost;
		this->OnCostChangedDel.Broadcast();
	}

	
	/* Events Dispatchers */

	UPROPERTY(BlueprintAssignable)
	FOnCostChanged OnCostChangedDel;

	UPROPERTY(BlueprintAssignable)
	FOnActionTextChanged OnActionTextChangedDel;

	UPROPERTY(BlueprintAssignable)
	FOnObjectNameTextChanged OnObjectNameTextChanged;

protected:
	// Action text is the text to show for the user added to the default text
	// e.g. Press [F] to buy random weapon from the Mystery Box 1000$
	UPROPERTY(BlueprintReadOnly)
	FText ActionText;
	
	UPROPERTY(BlueprintReadOnly)
	int32 Cost;

	UPROPERTY(BlueprintReadOnly)
	FText ObjectNameText;

	UPROPERTY(BlueprintReadOnly)
	FText AdditionalText;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor ObjectNameColor;
};
