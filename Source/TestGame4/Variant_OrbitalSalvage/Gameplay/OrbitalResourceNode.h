// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbitalTypes.h"
#include "OrbitalResourceNode.generated.h"

class UStaticMeshComponent;

/**
 * Harvestable asteroid/wreck node for mining and salvage.
 */
UCLASS()
class AOrbitalResourceNode : public AActor
{
	GENERATED_BODY()

public:
	AOrbitalResourceNode();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource")
	EOrbitalResourceNodeKind NodeKind = EOrbitalResourceNodeKind::Asteroid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource")
	EOrbitalResourceType ResourceType = EOrbitalResourceType::Ore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource")
	float ResourceUnitsRemaining = 320.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource")
	float UnitVolume = 1.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource")
	int32 UnitCreditValue = 18;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Resource")
	float MiningResistance = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
	bool bContainsBlackBox = false;

	UFUNCTION(BlueprintCallable, Category="Resource")
	float ExtractResource(float RequestedUnits, EOrbitalResourceType& OutResourceType, float& OutUnitVolume, int32& OutCreditValue, bool& bOutRecoveredBlackBox);
};

