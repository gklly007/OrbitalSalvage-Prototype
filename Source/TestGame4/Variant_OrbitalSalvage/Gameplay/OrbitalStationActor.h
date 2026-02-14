// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbitalStationActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class AOrbitalShipPawn;

/**
 * Dock target for trade/repair/refuel and extraction.
 */
UCLASS()
class AOrbitalStationActor : public AActor
{
	GENERATED_BODY()

public:
	AOrbitalStationActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USphereComponent* DockRangeSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Station")
	bool bIsExtractionBeacon = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Station")
	float DockRange = 1400.0f;

	UFUNCTION(BlueprintPure, Category="Station")
	bool CanDockShip(const AOrbitalShipPawn* Ship) const;
};

