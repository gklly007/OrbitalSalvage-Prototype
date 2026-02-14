// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbitalTypes.h"
#include "OrbitalSectorManager.generated.h"

class AOrbitalResourceNode;
class AOrbitalStationActor;
class AOrbitalJumpGateActor;
class AOrbitalEnemyDrone;

/**
 * Spawns and swaps active sector gameplay actors.
 */
UCLASS()
class AOrbitalSectorManager : public AActor
{
	GENERATED_BODY()

public:
	AOrbitalSectorManager();

	UFUNCTION(BlueprintCallable, Category="Sector")
	void LoadSector(EOrbitalSectorId SectorId);

	UFUNCTION(BlueprintPure, Category="Sector")
	FVector GetSectorSpawnPoint(EOrbitalSectorId SectorId) const;

	UFUNCTION(BlueprintPure, Category="Sector")
	AOrbitalStationActor* GetPrimaryStation() const { return PrimaryStation; }

	UFUNCTION(BlueprintPure, Category="Sector")
	AOrbitalStationActor* GetExtractionBeacon() const { return ExtractionBeacon; }

	UFUNCTION(BlueprintPure, Category="Sector")
	AOrbitalJumpGateActor* GetJumpGate() const { return JumpGate; }

private:
	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedActors;

	UPROPERTY()
	TObjectPtr<AOrbitalStationActor> PrimaryStation;

	UPROPERTY()
	TObjectPtr<AOrbitalStationActor> ExtractionBeacon;

	UPROPERTY()
	TObjectPtr<AOrbitalJumpGateActor> JumpGate;

	UPROPERTY(EditAnywhere, Category="Sector")
	FVector BeltCenter = FVector(0.0f, 0.0f, 240.0f);

	UPROPERTY(EditAnywhere, Category="Sector")
	FVector RuinsCenter = FVector(26000.0f, 0.0f, 240.0f);

	UPROPERTY(EditAnywhere, Category="Sector")
	float FieldRadius = 9000.0f;

	void ClearSpawnedActors();
	void SpawnBeltSector();
	void SpawnRuinsSector();

	void SpawnResourceField(const FVector& Center, int32 AsteroidCount, int32 WreckCount, bool bIncludeMissionWreck);
	void SpawnEnemyDrones(const FVector& Center, int32 Count);
};

