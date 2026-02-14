// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OrbitalTypes.generated.h"

UENUM(BlueprintType)
enum class EOrbitalResourceType : uint8
{
	Ore			UMETA(DisplayName = "Ore"),
	Salvage		UMETA(DisplayName = "Salvage"),
	FuelCell	UMETA(DisplayName = "Fuel Cell"),
	BlackBox	UMETA(DisplayName = "Black Box")
};

UENUM(BlueprintType)
enum class EOrbitalUpgradeType : uint8
{
	Reactor		UMETA(DisplayName = "Reactor"),
	CargoPod	UMETA(DisplayName = "Cargo Pod"),
	MiningLaser UMETA(DisplayName = "Mining Laser")
};

UENUM(BlueprintType)
enum class EOrbitalSectorId : uint8
{
	Belt		UMETA(DisplayName = "Belt"),
	Ruins		UMETA(DisplayName = "Ruins")
};

UENUM(BlueprintType)
enum class EOrbitalMissionStage : uint8
{
	RetrieveBlackBox	UMETA(DisplayName = "Recover Black Box"),
	JumpToRuins			UMETA(DisplayName = "Jump To Ruins"),
	ExtractAtBeacon		UMETA(DisplayName = "Extract At Beacon"),
	Completed			UMETA(DisplayName = "Completed")
};

UENUM(BlueprintType)
enum class EOrbitalResourceNodeKind : uint8
{
	Asteroid	UMETA(DisplayName = "Asteroid"),
	Wreck		UMETA(DisplayName = "Wreck")
};

