// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OrbitalTypes.h"
#include "OrbitalGameMode.generated.h"

class AOrbitalShipPawn;
class AOrbitalSectorManager;
class AOrbitalStationActor;
class AOrbitalJumpGateActor;
class UMissionComponent;

/**
 * Main game mode for the Orbital Salvage prototype loop.
 */
UCLASS()
class AOrbitalGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AOrbitalGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mission")
	UMissionComponent* MissionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Sector")
	EOrbitalSectorId CurrentSector = EOrbitalSectorId::Belt;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Sector")
	AOrbitalSectorManager* SectorManager = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Game")
	FString LastStatusMessage;

public:
	UFUNCTION(BlueprintCallable, Category="Game")
	void HandleBlackBoxRecovered();

	UFUNCTION(BlueprintCallable, Category="Game")
	void HandleDockRequest(AOrbitalStationActor* Station, AOrbitalShipPawn* Ship);

	UFUNCTION(BlueprintCallable, Category="Game")
	void HandleJumpGateRequest(AOrbitalJumpGateActor* Gate, AOrbitalShipPawn* Ship);

	UFUNCTION(BlueprintCallable, Category="Game")
	bool TryPurchaseUpgrade(EOrbitalUpgradeType UpgradeType);

	UFUNCTION(BlueprintPure, Category="Game")
	FText GetObjectiveText() const;

	UFUNCTION(BlueprintPure, Category="Game")
	FString GetCurrentSectorName() const;

private:
	float StatusMessageTimer = 0.0f;
	bool bPendingShipPlacement = false;

	void SetStatusMessage(const FString& Message, float DurationSeconds = 3.0f);
	AOrbitalShipPawn* GetPlayerShip() const;
	void LoadCurrentSectorAndPlaceShip(bool bForceResetVelocity);
};
