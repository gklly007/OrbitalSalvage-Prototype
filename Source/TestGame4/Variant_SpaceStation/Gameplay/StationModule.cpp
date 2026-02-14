// Copyright Epic Games, Inc. All Rights Reserved.

#include "StationModule.h"
#include "StationGrid.h"
#include "SpaceStationGameMode.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"

AStationModule::AStationModule()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Create mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Disable collision for preview

	// Default connection points (4-way corridor)
	ConnectionPoints = {
		FIntPoint(1, 0),   // Right
		FIntPoint(-1, 0),  // Left
		FIntPoint(0, 1),   // Forward
		FIntPoint(0, -1)   // Backward
	};
}

void AStationModule::SetPreviewMode(bool bPreview)
{
	if (MeshComponent)
	{
		if (bPreview)
		{
			// Make semi-transparent for preview
			MeshComponent->SetRenderCustomDepth(true);
			MeshComponent->SetCustomDepthStencilValue(1);

			// Create dynamic material if needed
			if (!DynamicMaterial && MeshComponent->GetMaterial(0))
			{
				DynamicMaterial = MeshComponent->CreateDynamicMaterialInstance(0);
			}

			if (DynamicMaterial)
			{
				DynamicMaterial->SetScalarParameterValue(FName("Opacity"), 0.5f);
			}
		}
		else
		{
			// Make opaque for placed module
			MeshComponent->SetRenderCustomDepth(false);

			if (DynamicMaterial)
			{
				DynamicMaterial->SetScalarParameterValue(FName("Opacity"), 1.0f);
			}

			// Enable collision for placed modules (crew can walk on them)
			MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
			MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Crew can overlap
		}
	}

	bIsPlaced = !bPreview;
}

void AStationModule::SetValidPlacement(bool bValid)
{
	if (MeshComponent && DynamicMaterial)
	{
		// Green for valid, red for invalid
		FLinearColor TintColor = bValid ? FLinearColor::Green : FLinearColor::Red;
		DynamicMaterial->SetVectorParameterValue(FName("TintColor"), TintColor);
	}
}

void AStationModule::SetPoweredState(bool bPowered)
{
	bIsPowered = bPowered;

	// Call Blueprint event for visual feedback
	BP_PowerStateChanged(bPowered);
}

bool AStationModule::CanConnectTo(AStationModule* Other) const
{
	if (!Other)
		return false;

	// Simple connectivity check - just need to be adjacent
	// More complex logic can be added in derived classes
	return true;
}

void AStationModule::UpdateConnections()
{
	ConnectedModules.Empty();

	// Get the station grid from the game mode
	ASpaceStationGameMode* GM = Cast<ASpaceStationGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM)
	{
		BP_ConnectionUpdated();
		return;
	}

	AStationGrid* Grid = GM->GetStationGrid();
	if (!Grid || !bIsPlaced)
	{
		BP_ConnectionUpdated();
		return;
	}

	// Check each connection point
	for (const FIntPoint& ConnectionOffset : ConnectionPoints)
	{
		// Rotate the offset based on module rotation
		FIntPoint WorldOffset = RotateOffset(ConnectionOffset, GridRotation);
		FIntPoint CheckCoord = GridPosition + WorldOffset;

		// Get module at that position
		AStationModule* AdjacentModule = Grid->GetModuleAt(CheckCoord);
		if (AdjacentModule && AdjacentModule != this && !ConnectedModules.Contains(AdjacentModule))
		{
			if (CanConnectTo(AdjacentModule))
			{
				ConnectedModules.Add(AdjacentModule);
			}
		}
	}

	// Blueprint event for visual feedback
	BP_ConnectionUpdated();
}

FIntPoint AStationModule::GetRotatedSize(const FRotator& Rotation) const
{
	// Get yaw angle
	float Yaw = Rotation.Yaw;

	// Normalize to 0-360
	while (Yaw < 0.0f)
		Yaw += 360.0f;
	while (Yaw >= 360.0f)
		Yaw -= 360.0f;

	// Check if rotated 90 or 270 degrees (swap X and Y)
	if (FMath::IsNearlyEqual(Yaw, 90.0f, 1.0f) || FMath::IsNearlyEqual(Yaw, 270.0f, 1.0f))
	{
		return FIntPoint(GridSize.Y, GridSize.X); // Swapped
	}

	return GridSize; // Normal or 180 degrees
}

FIntPoint AStationModule::RotateOffset(const FIntPoint& Offset, const FRotator& Rotation) const
{
	// Get yaw angle
	float Yaw = Rotation.Yaw;

	// Normalize to 0-360
	while (Yaw < 0.0f)
		Yaw += 360.0f;
	while (Yaw >= 360.0f)
		Yaw -= 360.0f;

	// Rotate offset based on yaw
	if (FMath::IsNearlyEqual(Yaw, 0.0f, 1.0f))
	{
		return Offset; // No rotation
	}
	else if (FMath::IsNearlyEqual(Yaw, 90.0f, 1.0f))
	{
		return FIntPoint(-Offset.Y, Offset.X); // 90 degrees CW
	}
	else if (FMath::IsNearlyEqual(Yaw, 180.0f, 1.0f))
	{
		return FIntPoint(-Offset.X, -Offset.Y); // 180 degrees
	}
	else if (FMath::IsNearlyEqual(Yaw, 270.0f, 1.0f))
	{
		return FIntPoint(Offset.Y, -Offset.X); // 270 degrees CW (90 CCW)
	}

	return Offset; // Default
}
