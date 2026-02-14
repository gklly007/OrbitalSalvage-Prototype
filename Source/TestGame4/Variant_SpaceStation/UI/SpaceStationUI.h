// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpaceStationUI.generated.h"

class UStationSystemsComponent;
class ASpaceStationGameMode;

/**
 * Base widget for the main Space Station HUD overlay.
 * Displays resource bars, crew status, and system warnings.
 * Extend this in Blueprint (WBP_SpaceStationHUD) for visual layout.
 */
UCLASS(abstract)
class USpaceStationUI : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Update the HUD display with current game state */
	UFUNCTION(BlueprintCallable, Category="UI")
	void RefreshDisplay();

	// Resource Display Values (bind these in Blueprint)

	/** Current net power (generation - consumption) */
	UPROPERTY(BlueprintReadOnly, Category="Resources")
	int32 DisplayPower = 0;

	/** Total power generation */
	UPROPERTY(BlueprintReadOnly, Category="Resources")
	int32 DisplayPowerGeneration = 0;

	/** Total power consumption */
	UPROPERTY(BlueprintReadOnly, Category="Resources")
	int32 DisplayPowerConsumption = 0;

	/** Is power sufficient */
	UPROPERTY(BlueprintReadOnly, Category="Resources")
	bool bDisplayPowerSufficient = true;

	/** Current net oxygen */
	UPROPERTY(BlueprintReadOnly, Category="Resources")
	int32 DisplayOxygen = 0;

	/** Total oxygen generation */
	UPROPERTY(BlueprintReadOnly, Category="Resources")
	int32 DisplayOxygenGeneration = 0;

	/** Total oxygen consumption */
	UPROPERTY(BlueprintReadOnly, Category="Resources")
	int32 DisplayOxygenConsumption = 0;

	/** Is oxygen sufficient */
	UPROPERTY(BlueprintReadOnly, Category="Resources")
	bool bDisplayOxygenSufficient = true;

	/** Current food supply */
	UPROPERTY(BlueprintReadOnly, Category="Resources")
	int32 DisplayFood = 0;

	/** Current credits */
	UPROPERTY(BlueprintReadOnly, Category="Resources")
	int32 DisplayCredits = 0;

	/** Number of placed modules */
	UPROPERTY(BlueprintReadOnly, Category="Station")
	int32 DisplayModuleCount = 0;

	/** Number of crew members */
	UPROPERTY(BlueprintReadOnly, Category="Station")
	int32 DisplayCrewCount = 0;

	/** Is the player currently in build mode */
	UPROPERTY(BlueprintReadOnly, Category="Station")
	bool bDisplayInBuildMode = false;

	/** Is the player currently in delete mode */
	UPROPERTY(BlueprintReadOnly, Category="Station")
	bool bDisplayInDeleteMode = false;

	/** Current game speed */
	UPROPERTY(BlueprintReadOnly, Category="Time")
	float DisplayGameSpeed = 1.0f;

	/** Is game paused */
	UPROPERTY(BlueprintReadOnly, Category="Time")
	bool bDisplayGamePaused = false;

	/** Number of selected crew */
	UPROPERTY(BlueprintReadOnly, Category="Station")
	int32 DisplaySelectedCrewCount = 0;

	/** System warning message (empty if no warning) */
	UPROPERTY(BlueprintReadOnly, Category="Warnings")
	FText DisplayWarningText;

	// Blueprint Events

	/** Called when resource values are updated */
	UFUNCTION(BlueprintImplementableEvent, Category="UI")
	void BP_OnResourcesUpdated();

	/** Called when a system warning should be shown */
	UFUNCTION(BlueprintImplementableEvent, Category="UI")
	void BP_OnWarning(const FText& WarningText);

	/** Called when warning should be cleared */
	UFUNCTION(BlueprintImplementableEvent, Category="UI")
	void BP_OnWarningCleared();

protected:

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	/** Timer for periodic refresh */
	float RefreshTimer = 0.0f;

	/** Refresh interval */
	float RefreshInterval = 0.25f;

	/** Previous warning state for change detection */
	bool bHadWarning = false;
};
