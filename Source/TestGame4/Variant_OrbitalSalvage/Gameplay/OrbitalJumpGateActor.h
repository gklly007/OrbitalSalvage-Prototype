// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbitalTypes.h"
#include "OrbitalJumpGateActor.generated.h"

class UStaticMeshComponent;
class AOrbitalShipPawn;

/**
 * Jump interaction point used to move between sectors.
 */
UCLASS()
class AOrbitalJumpGateActor : public AActor
{
	GENERATED_BODY()

public:
	AOrbitalJumpGateActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jump")
	EOrbitalSectorId TargetSector = EOrbitalSectorId::Belt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jump")
	float ActivationRange = 1200.0f;

	UFUNCTION(BlueprintPure, Category="Jump")
	bool CanShipUseGate(const AOrbitalShipPawn* Ship) const;
};

