// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpaceStationPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UFloatingPawnMovement;

/**
 * Floating camera platform with isometric 2.5D perspective for space station management.
 * Uses a spring arm for smooth camera positioning and zoom control.
 */
UCLASS(abstract)
class ASpaceStationPawn : public APawn
{
	GENERATED_BODY()

	/** Spring Arm for camera positioning */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	/** Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** Movement Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UFloatingPawnMovement* FloatingPawnMovement;

public:

	/** Constructor */
	ASpaceStationPawn();

	/** Minimum zoom level (spring arm length) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float MinZoomLevel = 1500.0f;

	/** Maximum zoom level (spring arm length) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float MaxZoomLevel = 4000.0f;

	/** Zoom interpolation speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float ZoomInterpSpeed = 8.0f;

	/** Target zoom level for smooth interpolation */
	float TargetZoomLevel = 2500.0f;

	/** Enable camera bounds clamping */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	bool bEnableCameraBounds = true;

	/** Maximum distance from origin the camera can pan */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float CameraBoundsRadius = 5000.0f;

	virtual void Tick(float DeltaSeconds) override;

public:

	/** Sets the camera zoom level by adjusting spring arm length */
	UFUNCTION(BlueprintCallable, Category="Camera")
	void SetZoomLevel(float ZoomLevel);

	/** Gets the current zoom level */
	UFUNCTION(BlueprintPure, Category="Camera")
	float GetCurrentZoom() const;

	/** Returns the camera component */
	UFUNCTION(BlueprintPure, Category="Camera")
	UCameraComponent* GetCamera() const { return Camera; }

	/** Returns the spring arm component */
	UFUNCTION(BlueprintPure, Category="Camera")
	USpringArmComponent* GetSpringArm() const { return SpringArm; }
};
