// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpaceStationPawn.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

ASpaceStationPawn::ASpaceStationPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create the root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Create the spring arm for isometric camera positioning
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeRotation(FRotator(-35.0f, 45.0f, 0.0f)); // Isometric angle
	SpringArm->TargetArmLength = 2500.0f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 3.0f;

	// Create the camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->SetFieldOfView(90.0f); // Wide FOV for isometric view
	Camera->ProjectionMode = ECameraProjectionMode::Perspective;

	// Create the movement component
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
	FloatingPawnMovement->bConstrainToPlane = true;
	FloatingPawnMovement->SetPlaneConstraintNormal(FVector::UpVector);
	FloatingPawnMovement->MaxSpeed = 2000.0f;
}

void ASpaceStationPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Smooth zoom interpolation
	if (SpringArm)
	{
		float CurrentLength = SpringArm->TargetArmLength;
		if (!FMath::IsNearlyEqual(CurrentLength, TargetZoomLevel, 1.0f))
		{
			float NewLength = FMath::FInterpTo(CurrentLength, TargetZoomLevel, DeltaSeconds, ZoomInterpSpeed);
			SpringArm->TargetArmLength = NewLength;
		}
	}

	// Camera bounds clamping
	if (bEnableCameraBounds)
	{
		FVector Location = GetActorLocation();
		float DistFromOrigin = FVector::Dist2D(Location, FVector::ZeroVector);
		if (DistFromOrigin > CameraBoundsRadius)
		{
			FVector Direction = Location;
			Direction.Z = 0.0f;
			Direction.Normalize();
			Location.X = Direction.X * CameraBoundsRadius;
			Location.Y = Direction.Y * CameraBoundsRadius;
			SetActorLocation(Location);
		}
	}
}

void ASpaceStationPawn::SetZoomLevel(float ZoomLevel)
{
	// Clamp and set target (smooth interpolation happens in Tick)
	TargetZoomLevel = FMath::Clamp(ZoomLevel, MinZoomLevel, MaxZoomLevel);
}

float ASpaceStationPawn::GetCurrentZoom() const
{
	return TargetZoomLevel;
}
