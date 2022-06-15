// Mahdi Abu Hamida - Final University Project (2022)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/GameplayStatics.h"
#include "MainMenuGameState.generated.h"

/**
 * 
 */
UCLASS()
class BLOODYSOULSTOWN_API AMainMenuGameState final : public AGameStateBase
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Gamepad Settings")
	void SetGamepadRightStickDeadZone(const float Value) const
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

	UFUNCTION(BlueprintCallable, Category = "Gamepad Settings")
	void SetGamepadLeftStickDeadZone(const float Value) const
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
};
