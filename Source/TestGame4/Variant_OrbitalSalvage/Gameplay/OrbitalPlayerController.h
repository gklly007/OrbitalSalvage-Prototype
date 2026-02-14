// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OrbitalTypes.h"
#include "OrbitalPlayerController.generated.h"

class AOrbitalShipPawn;
class AOrbitalGameMode;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 * Player input orchestration for the Orbital Salvage mode.
 */
UCLASS()
class AOrbitalPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AOrbitalPlayerController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;

protected:
	UPROPERTY()
	TObjectPtr<AOrbitalShipPawn> ControlledShip;

	UPROPERTY()
	TObjectPtr<AOrbitalGameMode> OrbitalGameMode;

	UPROPERTY(EditAnywhere, Category="Camera")
	float CameraZoomSpeed = 250.0f;

	// Enhanced input objects (created at runtime if no assets assigned).
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> FlightMappingContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> TurnAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MineAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> BrakeAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ToggleAssistAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ZoomAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> UpgradeReactorAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> UpgradeCargoAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> UpgradeMiningAction;

private:
	void MoveInput(const FInputActionValue& Value);
	void TurnInput(const FInputActionValue& Value);
	void MineStarted(const FInputActionValue& Value);
	void MineCompleted(const FInputActionValue& Value);
	void BrakeStarted(const FInputActionValue& Value);
	void BrakeCompleted(const FInputActionValue& Value);
	void InteractPressed(const FInputActionValue& Value);
	void ToggleAssistPressed(const FInputActionValue& Value);
	void ZoomInput(const FInputActionValue& Value);
	void UpgradeReactorPressed(const FInputActionValue& Value);
	void UpgradeCargoPressed(const FInputActionValue& Value);
	void UpgradeMiningPressed(const FInputActionValue& Value);

	void EnsureRuntimeInputSetup();
};

