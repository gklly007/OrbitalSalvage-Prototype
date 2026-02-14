// Copyright Epic Games, Inc. All Rights Reserved.

#include "StationGrid.h"
#include "StationModule.h"
#include "SpaceStationGameMode.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AStationGrid::AStationGrid()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AStationGrid::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Draw debug grid if enabled
	if (bShowGrid)
	{
		DrawDebugGrid();
	}
}

FIntPoint AStationGrid::WorldToGrid(const FVector& WorldLocation) const
{
	FVector RelativeLocation = WorldLocation - GridOrigin;
	int32 GridX = FMath::RoundToInt(RelativeLocation.X / TileSize);
	int32 GridY = FMath::RoundToInt(RelativeLocation.Y / TileSize);
	return FIntPoint(GridX, GridY);
}

FVector AStationGrid::GridToWorld(const FIntPoint& GridCoord) const
{
	FVector WorldLocation = GridOrigin;
	WorldLocation.X += GridCoord.X * TileSize;
	WorldLocation.Y += GridCoord.Y * TileSize;
	return WorldLocation;
}

bool AStationGrid::IsGridPositionOccupied(const FIntPoint& GridCoord, const FIntPoint& Size) const
{
	// Check if all tiles in the module's footprint are free
	for (int32 X = 0; X < Size.X; ++X)
	{
		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			FIntPoint CheckCoord = GridCoord + FIntPoint(X, Y);
			if (GridMap.Contains(CheckCoord))
			{
				return true; // Occupied
			}
		}
	}
	return false; // All tiles free
}

bool AStationGrid::IsValidPlacement(AStationModule* Module, const FIntPoint& GridCoord, const FRotator& Rotation)
{
	if (!Module)
		return false;

	// Check if space is unoccupied
	FIntPoint Size = Module->GetRotatedSize(Rotation);
	if (IsGridPositionOccupied(GridCoord, Size))
		return false;

	// Check connectivity (if not first module)
	if (GridMap.Num() > 0)
	{
		if (!CheckAdjacentConnection(GridCoord, Size))
			return false;
	}

	return true;
}

bool AStationGrid::PlaceModule(AStationModule* Module, const FIntPoint& GridCoord, const FRotator& Rotation)
{
	if (!Module || !IsValidPlacement(Module, GridCoord, Rotation))
		return false;

	// Set module properties
	Module->GridPosition = GridCoord;
	Module->GridRotation = Rotation;
	Module->bIsPlaced = true;

	// Position module in world
	FVector WorldPos = GridToWorld(GridCoord);
	Module->SetActorLocation(WorldPos);
	Module->SetActorRotation(Rotation);

	// Add to grid map
	FIntPoint Size = Module->GetRotatedSize(Rotation);
	for (int32 X = 0; X < Size.X; ++X)
	{
		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			FIntPoint MapCoord = GridCoord + FIntPoint(X, Y);
			GridMap.Add(MapCoord, Module);
		}
	}

	// Update connections on the new module
	Module->UpdateConnections();

	// Update connections on neighboring modules so they know about the new module
	for (AStationModule* Connected : Module->ConnectedModules)
	{
		if (Connected)
		{
			Connected->UpdateConnections();
		}
	}

	return true;
}

void AStationGrid::RemoveModule(const FIntPoint& GridCoord)
{
	AStationModule* Module = GetModuleAt(GridCoord);
	if (!Module)
		return;

	// Unregister from game mode before removing
	if (ASpaceStationGameMode* GM = Cast<ASpaceStationGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GM->UnregisterModule(Module);
	}

	// Remove from grid map
	FIntPoint Size = Module->GetRotatedSize(Module->GridRotation);
	for (int32 X = 0; X < Size.X; ++X)
	{
		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			FIntPoint MapCoord = Module->GridPosition + FIntPoint(X, Y);
			GridMap.Remove(MapCoord);
		}
	}

	// Update connections on neighboring modules
	for (AStationModule* Connected : Module->ConnectedModules)
	{
		if (Connected)
		{
			Connected->UpdateConnections();
		}
	}

	// Destroy module
	Module->Destroy();
}

AStationModule* AStationGrid::GetModuleAt(const FIntPoint& GridCoord) const
{
	AStationModule* const* FoundModule = GridMap.Find(GridCoord);
	return FoundModule ? *FoundModule : nullptr;
}

TArray<AStationModule*> AStationGrid::GetAdjacentModules(const FIntPoint& GridCoord) const
{
	TArray<AStationModule*> Adjacent;

	// Check four cardinal directions
	const FIntPoint Offsets[] = {
		FIntPoint(1, 0),   // Right
		FIntPoint(-1, 0),  // Left
		FIntPoint(0, 1),   // Forward
		FIntPoint(0, -1)   // Backward
	};

	for (const FIntPoint& Offset : Offsets)
	{
		AStationModule* Module = GetModuleAt(GridCoord + Offset);
		if (Module && !Adjacent.Contains(Module))
		{
			Adjacent.Add(Module);
		}
	}

	return Adjacent;
}

void AStationGrid::DrawDebugGrid_Implementation()
{
	if (!GetWorld())
		return;

	// Draw grid lines
	float LineLifetime = 0.0f; // Draw for one frame
	float LineThickness = 2.0f;

	int32 HalfSize = DebugGridSize / 2;

	// Draw horizontal lines
	for (int32 Y = -HalfSize; Y <= HalfSize; ++Y)
	{
		FVector Start = GridToWorld(FIntPoint(-HalfSize, Y));
		FVector End = GridToWorld(FIntPoint(HalfSize, Y));
		DrawDebugLine(GetWorld(), Start, End, GridColor, false, LineLifetime, 0, LineThickness);
	}

	// Draw vertical lines
	for (int32 X = -HalfSize; X <= HalfSize; ++X)
	{
		FVector Start = GridToWorld(FIntPoint(X, -HalfSize));
		FVector End = GridToWorld(FIntPoint(X, HalfSize));
		DrawDebugLine(GetWorld(), Start, End, GridColor, false, LineLifetime, 0, LineThickness);
	}
}

bool AStationGrid::CheckAdjacentConnection(const FIntPoint& GridCoord, const FIntPoint& Size) const
{
	// Check if any tile in the module's footprint is adjacent to an existing module
	for (int32 X = 0; X < Size.X; ++X)
	{
		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			FIntPoint CheckCoord = GridCoord + FIntPoint(X, Y);
			TArray<AStationModule*> Adjacent = GetAdjacentModules(CheckCoord);
			if (Adjacent.Num() > 0)
			{
				return true; // Connected to at least one module
			}
		}
	}
	return false; // Not connected to any module
}
