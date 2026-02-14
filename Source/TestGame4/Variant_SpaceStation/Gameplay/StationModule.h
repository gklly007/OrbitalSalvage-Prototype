// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StationModule.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;

/**
 * Enum for module types - used by AI to find appropriate modules.
 */
UENUM(BlueprintType)
enum class EStationModuleType : uint8
{
	Corridor,       // Basic connection corridor
	PowerGenerator, // Generates power
	LifeSupport,    // Generates oxygen/atmosphere
	Quarters,       // Crew sleeping area
	MessHall,       // Food consumption area
	Storage,        // Resource storage
	MedBay,         // Health restoration
	Command,        // Station command center
	Custom          // Custom/other
};

/**
 * Base class for all placeable station modules (rooms, corridors, etc.).
 * Handles grid placement, connectivity, power/atmosphere, and visual feedback.
 */
UCLASS(abstract)
class AStationModule : public AActor
{
	GENERATED_BODY()

protected:

	/** Main mesh component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MeshComponent;

	/** Dynamic material for visual feedback */
	UMaterialInstanceDynamic* DynamicMaterial;

public:

	// Grid Properties

	/** Size of module in grid tiles (e.g., 1x1, 2x2, 2x3) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	FIntPoint GridSize = FIntPoint(1, 1);

	/** Current grid position (set by grid when placed) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Grid")
	FIntPoint GridPosition = FIntPoint(0, 0);

	/** Current grid rotation (0, 90, 180, or 270 degrees) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Grid")
	FRotator GridRotation = FRotator::ZeroRotator;

	/** Is this module placed on the grid (vs preview) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Grid")
	bool bIsPlaced = false;

	// Module Properties

	/** Type of this module */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Module")
	EStationModuleType ModuleType = EStationModuleType::Corridor;

	/** Resource cost to build this module */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Module")
	int32 BuildCost = 100;

	/** Display name of the module */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Module")
	FText ModuleName = FText::FromString("Module");

	/** Description of the module */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Module", meta=(MultiLine=true))
	FText ModuleDescription;

	/** Icon for UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Module")
	UTexture2D* ModuleIcon;

	// System Properties

	/** Power generated per tick (0 for most modules) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Systems")
	int32 PowerGeneration = 0;

	/** Power consumed per tick */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Systems")
	int32 PowerConsumption = 0;

	/** Oxygen generated per tick (0 for most modules) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Systems")
	int32 OxygenGeneration = 0;

	/** Oxygen consumed per tick */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Systems")
	int32 OxygenConsumption = 0;

	/** Max crew this module can support */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Systems")
	int32 CrewCapacity = 0;

	/** Does this module require power to function */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Systems")
	bool bRequiresPower = true;

	/** Does this module require atmosphere to function */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Systems")
	bool bRequiresAtmosphere = true;

	/** Is this module currently powered */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Systems")
	bool bIsPowered = false;

	/** Does this module currently have atmosphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Systems")
	bool bHasAtmosphere = false;

	// Connectivity

	/** Connection points (relative grid offsets where modules can connect) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Connectivity")
	TArray<FIntPoint> ConnectionPoints;

	/** Adjacent connected modules */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Connectivity")
	TArray<AStationModule*> ConnectedModules;

public:

	/** Constructor */
	AStationModule();

	/** Visual State */

	/** Set module to preview mode (semi-transparent) */
	UFUNCTION(BlueprintCallable, Category="Module")
	void SetPreviewMode(bool bPreview);

	/** Set placement validity visual (green/red) */
	UFUNCTION(BlueprintCallable, Category="Module")
	void SetValidPlacement(bool bValid);

	/** Set powered state visual */
	UFUNCTION(BlueprintCallable, Category="Module")
	void SetPoweredState(bool bPowered);

	/** Connectivity */

	/** Check if this module can connect to another */
	UFUNCTION(BlueprintPure, Category="Connectivity")
	bool CanConnectTo(AStationModule* Other) const;

	/** Update connections with adjacent modules */
	UFUNCTION(BlueprintCallable, Category="Connectivity")
	void UpdateConnections();

	/** Get rotated size based on current rotation */
	UFUNCTION(BlueprintPure, Category="Grid")
	FIntPoint GetRotatedSize(const FRotator& Rotation) const;

	// Blueprint Events

	/** Called when module is placed */
	UFUNCTION(BlueprintImplementableEvent, Category="Module")
	void BP_ModulePlaced();

	/** Called when power state changes */
	UFUNCTION(BlueprintImplementableEvent, Category="Module")
	void BP_PowerStateChanged(bool bPowered);

	/** Called when connections update */
	UFUNCTION(BlueprintImplementableEvent, Category="Module")
	void BP_ConnectionUpdated();

protected:

	/** Rotate an offset based on module rotation */
	FIntPoint RotateOffset(const FIntPoint& Offset, const FRotator& Rotation) const;
};
