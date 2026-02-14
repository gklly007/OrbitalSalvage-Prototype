// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StationGrid.generated.h"

class AStationModule;

/**
 * Grid-based building system manager for the space station.
 * Manages module placement, validation, and grid coordinates.
 */
UCLASS()
class AStationGrid : public AActor
{
	GENERATED_BODY()

protected:

	/** Sparse grid storage - maps grid coordinates to placed modules */
	TMap<FIntPoint, AStationModule*> GridMap;

	/** Size of each grid tile in cm (200cm = 2m) */
	UPROPERTY(EditAnywhere, Category="Grid")
	int32 TileSize = 200;

	/** World position of grid origin (0,0) */
	UPROPERTY(EditAnywhere, Category="Grid")
	FVector GridOrigin = FVector::ZeroVector;

	/** Show debug grid lines */
	UPROPERTY(EditAnywhere, Category="Grid|Debug")
	bool bShowGrid = true;

	/** Grid line color */
	UPROPERTY(EditAnywhere, Category="Grid|Debug")
	FColor GridColor = FColor::Green;

	/** Grid size to draw (in tiles) */
	UPROPERTY(EditAnywhere, Category="Grid|Debug")
	int32 DebugGridSize = 50;

public:

	/** Constructor */
	AStationGrid();

	/** Tick for debug visualization */
	virtual void Tick(float DeltaSeconds) override;

public:

	/** Convert world location to grid coordinates */
	UFUNCTION(BlueprintPure, Category="Grid")
	FIntPoint WorldToGrid(const FVector& WorldLocation) const;

	/** Convert grid coordinates to world location */
	UFUNCTION(BlueprintPure, Category="Grid")
	FVector GridToWorld(const FIntPoint& GridCoord) const;

	/** Check if grid position is occupied */
	UFUNCTION(BlueprintPure, Category="Grid")
	bool IsGridPositionOccupied(const FIntPoint& GridCoord, const FIntPoint& Size) const;

	/** Check if placement is valid (unoccupied and connected) */
	UFUNCTION(BlueprintPure, Category="Grid")
	bool IsValidPlacement(AStationModule* Module, const FIntPoint& GridCoord, const FRotator& Rotation);

	/** Place a module on the grid */
	UFUNCTION(BlueprintCallable, Category="Grid")
	bool PlaceModule(AStationModule* Module, const FIntPoint& GridCoord, const FRotator& Rotation);

	/** Remove a module from the grid */
	UFUNCTION(BlueprintCallable, Category="Grid")
	void RemoveModule(const FIntPoint& GridCoord);

	/** Get module at grid position */
	UFUNCTION(BlueprintPure, Category="Grid")
	AStationModule* GetModuleAt(const FIntPoint& GridCoord) const;

	/** Get adjacent modules (for connectivity checks) */
	UFUNCTION(BlueprintPure, Category="Grid")
	TArray<AStationModule*> GetAdjacentModules(const FIntPoint& GridCoord) const;

protected:

	/** Draw debug grid lines */
	UFUNCTION(BlueprintNativeEvent, Category="Grid|Debug")
	void DrawDebugGrid();
	virtual void DrawDebugGrid_Implementation();

	/** Check if module is connected to existing station */
	bool CheckAdjacentConnection(const FIntPoint& GridCoord, const FIntPoint& Size) const;
};
