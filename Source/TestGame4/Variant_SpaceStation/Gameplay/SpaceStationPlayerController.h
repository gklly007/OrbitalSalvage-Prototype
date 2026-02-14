// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpaceStationPlayerController.generated.h"

class ASpaceStationPawn;
class ASpaceStationHUD;
class AStationGrid;
class AStationModule;
class ACrewMember;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 * Player Controller for space station management.
 * Handles building mode, crew commands, and camera controls.
 */
UCLASS(abstract)
class ASpaceStationPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** Space Station Pawn associated with this controller */
	ASpaceStationPawn* ControlledPawn;

	/** Space Station HUD associated with this controller */
	ASpaceStationHUD* SpaceStationHUD;

	/** Pointer to the station grid */
	AStationGrid* StationGrid;

	/** Input Mapping Context */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> SpaceStationMappingContext;

	// Input Actions
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> CameraPanAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> CameraZoomAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ResetCameraAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SelectAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> PlaceModuleAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> CancelPlacementAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> RotatePreviewAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> CommandCrewAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> OpenBuildMenuAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> DeleteModuleAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SpawnCrewAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> TogglePauseAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SpeedUpAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SpeedDownAction;

	/** Camera controls */
	UPROPERTY(EditAnywhere, Category="Camera")
	float ZoomSpeed = 100.0f;

	/** Enable edge scrolling when mouse is near screen edge */
	UPROPERTY(EditAnywhere, Category="Camera")
	bool bEnableEdgeScrolling = true;

	/** Distance from screen edge (in pixels) to trigger scrolling */
	UPROPERTY(EditAnywhere, Category="Camera")
	float EdgeScrollThreshold = 30.0f;

	/** Speed of edge scrolling */
	UPROPERTY(EditAnywhere, Category="Camera")
	float EdgeScrollSpeed = 15.0f;

	/** Auto-enter build mode on BeginPlay (for testing) */
	UPROPERTY(EditAnywhere, Category="Building")
	bool bAutoEnterBuildMode = true;

	/** Default module class to use when auto-entering build mode */
	UPROPERTY(EditAnywhere, Category="Building")
	TSubclassOf<AStationModule> DefaultBuildModuleClass;

	/** Building mode state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Building")
	bool bInBuildMode = false;

	/** Delete mode state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Building")
	bool bInDeleteMode = false;

	/** Selected module class to build */
	TSubclassOf<AStationModule> SelectedModuleClass;

	/** Preview module being placed */
	AStationModule* PreviewModule;

	/** Current rotation for preview module */
	FRotator PreviewRotation = FRotator::ZeroRotator;

	/** Selected crew members */
	TArray<ACrewMember*> SelectedCrew;

	/** Current game speed multiplier */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Time")
	float GameSpeed = 1.0f;

	/** Is the game currently paused */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Time")
	bool bGamePaused = false;

public:

	/** Constructor */
	ASpaceStationPlayerController();

	/** Setup input component */
	virtual void SetupInputComponent() override;

	/** Called when possessed pawn changes */
	virtual void OnPossess(APawn* InPawn) override;

	/** Called when gameplay begins */
	virtual void BeginPlay() override;

	/** Tick for build mode updates */
	virtual void Tick(float DeltaSeconds) override;

public:

	/** Enter building mode with specified module class */
	UFUNCTION(BlueprintCallable, Category="Building")
	void EnterBuildMode(TSubclassOf<AStationModule> ModuleClass);

	/** Exit building mode */
	UFUNCTION(BlueprintCallable, Category="Building")
	void ExitBuildMode();

	/** Update build preview (called in Tick when in build mode) */
	void UpdateBuildPreview();

	/** Place the current preview module */
	void PlaceModule();

	/** Rotate the preview module 90 degrees */
	void RotatePreview();

	/** Get grid location under cursor via raycast */
	bool GetGridLocationUnderCursor(FIntPoint& OutGridCoord);

	/** Select a crew member (additive = shift+click to add to selection) */
	void SelectCrew(ACrewMember* Crew, bool bAdditive = false);

	/** Deselect all crew */
	void DeselectAllCrew();

	/** Command selected crew to move to location */
	void CommandCrewMove(const FVector& Location);

	/** Spawn a crew member at cursor location */
	UFUNCTION(BlueprintCallable, Category="Crew")
	void SpawnCrewAtCursor();

	/** Toggle game pause */
	UFUNCTION(BlueprintCallable, Category="Time")
	void TogglePause();

	/** Set game speed multiplier */
	UFUNCTION(BlueprintCallable, Category="Time")
	void SetGameSpeed(float Speed);

	/** Get current game speed */
	UFUNCTION(BlueprintPure, Category="Time")
	float GetGameSpeed() const { return GameSpeed; }

	/** Is game paused */
	UFUNCTION(BlueprintPure, Category="Time")
	bool IsGamePaused() const { return bGamePaused; }

	/** Get number of selected crew */
	UFUNCTION(BlueprintPure, Category="Selection")
	int32 GetSelectedCrewCount() const { return SelectedCrew.Num(); }

	/** Is the player currently in build mode */
	UFUNCTION(BlueprintPure, Category="Building")
	bool IsInBuildMode() const { return bInBuildMode; }

	/** Enter delete mode (click modules to remove them) */
	UFUNCTION(BlueprintCallable, Category="Building")
	void EnterDeleteMode();

	/** Exit delete mode */
	UFUNCTION(BlueprintCallable, Category="Building")
	void ExitDeleteMode();

	/** Is the player currently in delete mode */
	UFUNCTION(BlueprintPure, Category="Building")
	bool IsInDeleteMode() const { return bInDeleteMode; }

	/** Delete the module under the cursor */
	void DeleteModuleUnderCursor();

	/** Update delete mode preview (highlight module under cursor) */
	void UpdateDeletePreview();

	/** Currently highlighted module for deletion */
	AStationModule* HighlightedModule = nullptr;

protected:

	// Input Handlers
	void CameraPan(const FInputActionValue& Value);
	void CameraZoom(const FInputActionValue& Value);
	void OnResetCamera(const FInputActionValue& Value);
	void OnSelect(const FInputActionValue& Value);
	void OnPlaceModule(const FInputActionValue& Value);
	void OnCancelPlacement(const FInputActionValue& Value);
	void OnRotatePreview(const FInputActionValue& Value);
	void OnCommandCrew(const FInputActionValue& Value);
	void OnOpenBuildMenu(const FInputActionValue& Value);
	void OnDeleteModule(const FInputActionValue& Value);
	void OnSpawnCrew(const FInputActionValue& Value);
	void OnTogglePause(const FInputActionValue& Value);
	void OnSpeedUp(const FInputActionValue& Value);
	void OnSpeedDown(const FInputActionValue& Value);

	/** Edge scrolling logic */
	void UpdateEdgeScrolling(float DeltaSeconds);
};
