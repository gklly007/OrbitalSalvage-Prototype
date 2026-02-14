// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SpaceStationHUD.generated.h"

class ACrewMember;
class AStationModule;

/**
 * HUD for space station management.
 * Manages UI widgets and draws visual overlays (selection, connections).
 */
UCLASS(abstract)
class ASpaceStationHUD : public AHUD
{
	GENERATED_BODY()

public:

	ASpaceStationHUD();

	virtual void DrawHUD() override;
	virtual void BeginPlay() override;

	// HUD Widget Class (set in Blueprint)

	/** Main HUD widget class to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<UUserWidget> MainHUDWidgetClass;

	/** Build menu widget class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<UUserWidget> BuildMenuWidgetClass;

	// Widget References

	/** Spawned main HUD widget */
	UPROPERTY(BlueprintReadOnly, Category="UI")
	UUserWidget* MainHUDWidget;

	/** Spawned build menu widget */
	UPROPERTY(BlueprintReadOnly, Category="UI")
	UUserWidget* BuildMenuWidget;

	// Toggle State

	/** Is the build menu currently visible */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	bool bBuildMenuVisible = false;

public:

	/** Toggle the build menu on/off */
	UFUNCTION(BlueprintCallable, Category="UI")
	void ToggleBuildMenu();

	/** Show the build menu */
	UFUNCTION(BlueprintCallable, Category="UI")
	void ShowBuildMenu();

	/** Hide the build menu */
	UFUNCTION(BlueprintCallable, Category="UI")
	void HideBuildMenu();

	/** Create and show the main HUD widget */
	UFUNCTION(BlueprintCallable, Category="UI")
	void CreateMainHUD();

protected:

	/** Draw selection boxes around selected crew */
	void DrawCrewSelection();

	/** Draw module connection lines */
	void DrawModuleConnections();

	/** Draw resource bars on screen */
	void DrawResourceOverlay();

	// Drawing helpers

	/** Draw a box around a world actor projected to screen */
	void DrawWorldActorBounds(AActor* Actor, FLinearColor Color, float Thickness = 2.0f);

	/** Project world location to screen */
	bool WorldToScreen(const FVector& WorldLocation, FVector2D& ScreenLocation) const;
};
