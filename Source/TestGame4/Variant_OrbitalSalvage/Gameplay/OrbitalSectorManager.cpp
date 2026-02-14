// Copyright Epic Games, Inc. All Rights Reserved.

#include "OrbitalSectorManager.h"
#include "OrbitalResourceNode.h"
#include "OrbitalStationActor.h"
#include "OrbitalJumpGateActor.h"
#include "OrbitalEnemyDrone.h"
#include "Engine/World.h"

AOrbitalSectorManager::AOrbitalSectorManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AOrbitalSectorManager::LoadSector(EOrbitalSectorId SectorId)
{
	ClearSpawnedActors();
	PrimaryStation = nullptr;
	ExtractionBeacon = nullptr;
	JumpGate = nullptr;

	switch (SectorId)
	{
	case EOrbitalSectorId::Belt:
		SpawnBeltSector();
		break;
	case EOrbitalSectorId::Ruins:
		SpawnRuinsSector();
		break;
	default:
		SpawnBeltSector();
		break;
	}
}

FVector AOrbitalSectorManager::GetSectorSpawnPoint(EOrbitalSectorId SectorId) const
{
	switch (SectorId)
	{
	case EOrbitalSectorId::Belt:
		return BeltCenter + FVector(-2500.0f, -1000.0f, 0.0f);
	case EOrbitalSectorId::Ruins:
		return RuinsCenter + FVector(-2600.0f, -1200.0f, 0.0f);
	default:
		return BeltCenter;
	}
}

void AOrbitalSectorManager::ClearSpawnedActors()
{
	for (AActor* SpawnedActor : SpawnedActors)
	{
		if (IsValid(SpawnedActor))
		{
			SpawnedActor->Destroy();
		}
	}
	SpawnedActors.Empty();
}

void AOrbitalSectorManager::SpawnBeltSector()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	PrimaryStation = World->SpawnActor<AOrbitalStationActor>(AOrbitalStationActor::StaticClass(), BeltCenter, FRotator::ZeroRotator);
	if (PrimaryStation)
	{
		PrimaryStation->bIsExtractionBeacon = false;
		SpawnedActors.Add(PrimaryStation);
	}

	JumpGate = World->SpawnActor<AOrbitalJumpGateActor>(AOrbitalJumpGateActor::StaticClass(), BeltCenter + FVector(7500.0f, 0.0f, 0.0f), FRotator::ZeroRotator);
	if (JumpGate)
	{
		JumpGate->TargetSector = EOrbitalSectorId::Ruins;
		SpawnedActors.Add(JumpGate);
	}

	SpawnResourceField(BeltCenter, 20, 6, true);
	SpawnEnemyDrones(BeltCenter + FVector(3500.0f, 2200.0f, 0.0f), 2);
}

void AOrbitalSectorManager::SpawnRuinsSector()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ExtractionBeacon = World->SpawnActor<AOrbitalStationActor>(AOrbitalStationActor::StaticClass(), RuinsCenter, FRotator::ZeroRotator);
	if (ExtractionBeacon)
	{
		ExtractionBeacon->bIsExtractionBeacon = true;
		SpawnedActors.Add(ExtractionBeacon);
	}

	JumpGate = World->SpawnActor<AOrbitalJumpGateActor>(AOrbitalJumpGateActor::StaticClass(), RuinsCenter + FVector(-7600.0f, 0.0f, 0.0f), FRotator::ZeroRotator);
	if (JumpGate)
	{
		JumpGate->TargetSector = EOrbitalSectorId::Belt;
		SpawnedActors.Add(JumpGate);
	}

	SpawnResourceField(RuinsCenter, 12, 10, false);
	SpawnEnemyDrones(RuinsCenter + FVector(1800.0f, 1200.0f, 0.0f), 5);
}

void AOrbitalSectorManager::SpawnResourceField(const FVector& Center, int32 AsteroidCount, int32 WreckCount, bool bIncludeMissionWreck)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	auto SpawnNode = [&](EOrbitalResourceNodeKind Kind, EOrbitalResourceType Type, float Units, float RadiusScale, bool bBlackBox)
	{
		const FVector RandOffset = FVector(
			FMath::FRandRange(-FieldRadius, FieldRadius),
			FMath::FRandRange(-FieldRadius, FieldRadius),
			0.0f
		);

		AOrbitalResourceNode* Node = World->SpawnActor<AOrbitalResourceNode>(AOrbitalResourceNode::StaticClass(), Center + RandOffset, FRotator::ZeroRotator);
		if (Node)
		{
			Node->NodeKind = Kind;
			Node->ResourceType = Type;
			Node->ResourceUnitsRemaining = Units;
			Node->bContainsBlackBox = bBlackBox;

			if (Kind == EOrbitalResourceNodeKind::Asteroid)
			{
				Node->UnitVolume = 1.5f;
				Node->UnitCreditValue = 16;
				Node->MiningResistance = 1.2f;
			}
			else
			{
				Node->UnitVolume = 1.0f;
				Node->UnitCreditValue = 24;
				Node->MiningResistance = 0.9f;
			}

			Node->SetActorScale3D(FVector(RadiusScale));
			SpawnedActors.Add(Node);
		}
	};

	for (int32 i = 0; i < AsteroidCount; ++i)
	{
		SpawnNode(EOrbitalResourceNodeKind::Asteroid, EOrbitalResourceType::Ore, FMath::FRandRange(180.0f, 360.0f), FMath::FRandRange(1.6f, 3.0f), false);
	}

	bool bMissionNodeSpawned = false;
	for (int32 i = 0; i < WreckCount; ++i)
	{
		const bool bMissionNode = bIncludeMissionWreck && !bMissionNodeSpawned && i == 0;
		SpawnNode(EOrbitalResourceNodeKind::Wreck, EOrbitalResourceType::Salvage, FMath::FRandRange(80.0f, 190.0f), FMath::FRandRange(1.1f, 2.0f), bMissionNode);
		bMissionNodeSpawned |= bMissionNode;
	}
}

void AOrbitalSectorManager::SpawnEnemyDrones(const FVector& Center, int32 Count)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (int32 i = 0; i < Count; ++i)
	{
		const FVector Offset(FMath::FRandRange(-1500.0f, 1500.0f), FMath::FRandRange(-1500.0f, 1500.0f), 0.0f);
		AOrbitalEnemyDrone* Drone = World->SpawnActor<AOrbitalEnemyDrone>(AOrbitalEnemyDrone::StaticClass(), Center + Offset, FRotator::ZeroRotator);
		if (Drone)
		{
			SpawnedActors.Add(Drone);
		}
	}
}

