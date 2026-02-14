// Copyright Epic Games, Inc. All Rights Reserved.

#include "OrbitalGameMode.h"
#include "OrbitalShipPawn.h"
#include "OrbitalPlayerController.h"
#include "OrbitalSectorManager.h"
#include "OrbitalStationActor.h"
#include "OrbitalJumpGateActor.h"
#include "OrbitalHUD.h"
#include "MissionComponent.h"
#include "ShipSystemsComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AOrbitalGameMode::AOrbitalGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultPawnClass = AOrbitalShipPawn::StaticClass();
	PlayerControllerClass = AOrbitalPlayerController::StaticClass();
	HUDClass = AOrbitalHUD::StaticClass();

	MissionComponent = CreateDefaultSubobject<UMissionComponent>(TEXT("MissionComponent"));
}

void AOrbitalGameMode::BeginPlay()
{
	Super::BeginPlay();

	SectorManager = GetWorld()->SpawnActor<AOrbitalSectorManager>(AOrbitalSectorManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	LoadCurrentSectorAndPlaceShip(true);
	bPendingShipPlacement = (GetPlayerShip() == nullptr);

	SetStatusMessage(TEXT("Launch complete. Mine wrecks and recover the black box."));
}

void AOrbitalGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (StatusMessageTimer > 0.0f)
	{
		StatusMessageTimer = FMath::Max(0.0f, StatusMessageTimer - DeltaSeconds);
		if (StatusMessageTimer <= 0.0f)
		{
			LastStatusMessage.Empty();
		}
	}

	if (bPendingShipPlacement && GetPlayerShip() != nullptr)
	{
		bPendingShipPlacement = false;
		LoadCurrentSectorAndPlaceShip(true);
	}
}

void AOrbitalGameMode::HandleBlackBoxRecovered()
{
	if (!MissionComponent)
	{
		return;
	}

	if (!MissionComponent->bHasRecoveredBlackBox)
	{
		MissionComponent->MarkBlackBoxRecovered();
		SetStatusMessage(TEXT("Black box recovered. Reach a jump gate to travel to the Ruins."));
	}
}

void AOrbitalGameMode::HandleDockRequest(AOrbitalStationActor* Station, AOrbitalShipPawn* Ship)
{
	if (!Station || !Ship || !Station->CanDockShip(Ship))
	{
		return;
	}

	UShipSystemsComponent* Systems = Ship->GetShipSystems();
	if (!Systems)
	{
		return;
	}

	// Stop ship drift when docked.
	if (Ship->ShipMesh)
	{
		Ship->ShipMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		Ship->ShipMesh->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
	}

	const int32 CargoRevenue = Systems->SellAllCargo();

	// Auto service operations using credits.
	const float HullMissing = Systems->MaxHull - Systems->Hull;
	const float FuelMissing = Systems->MaxFuel - Systems->Fuel;
	const int32 RepairCost = FMath::CeilToInt(HullMissing * 3.0f);
	const int32 FuelCost = FMath::CeilToInt(FuelMissing * 2.0f);
	const int32 ServiceCost = RepairCost + FuelCost;

	if (Systems->Credits >= ServiceCost)
	{
		Systems->Credits -= ServiceCost;
		Systems->RepairHull(HullMissing);
		Systems->Refuel(FuelMissing);
	}

	if (Station->bIsExtractionBeacon && MissionComponent)
	{
		int32 MissionReward = 0;
		if (MissionComponent->TryCompleteExtraction(MissionReward))
		{
			Systems->Credits += MissionReward;
			SetStatusMessage(FString::Printf(TEXT("Mission complete. Reward +%d credits."), MissionReward), 6.0f);
			return;
		}
	}

	SetStatusMessage(FString::Printf(TEXT("Docked. Cargo sold +%d cr. Service cost -%d cr."), CargoRevenue, ServiceCost));
}

void AOrbitalGameMode::HandleJumpGateRequest(AOrbitalJumpGateActor* Gate, AOrbitalShipPawn* Ship)
{
	if (!Gate || !Ship || !Gate->CanShipUseGate(Ship))
	{
		return;
	}

	if (MissionComponent &&
		MissionComponent->CurrentStage == EOrbitalMissionStage::RetrieveBlackBox &&
		Gate->TargetSector == EOrbitalSectorId::Ruins)
	{
		SetStatusMessage(TEXT("Mission lock: recover the black box before jumping to Ruins."));
		return;
	}

	CurrentSector = Gate->TargetSector;
	LoadCurrentSectorAndPlaceShip(true);

	if (MissionComponent)
	{
		MissionComponent->NotifyEnteredSector(CurrentSector);
	}

	SetStatusMessage(FString::Printf(TEXT("Jump complete. Entered sector: %s"), *GetCurrentSectorName()));
}

bool AOrbitalGameMode::TryPurchaseUpgrade(EOrbitalUpgradeType UpgradeType)
{
	if (AOrbitalShipPawn* Ship = GetPlayerShip())
	{
		if (UShipSystemsComponent* Systems = Ship->GetShipSystems())
		{
			const int32 Cost = Systems->GetUpgradeCost(UpgradeType);
			if (Systems->PurchaseUpgrade(UpgradeType))
			{
				SetStatusMessage(FString::Printf(TEXT("Upgrade installed (%d cr)."), Cost));
				return true;
			}

			SetStatusMessage(TEXT("Upgrade purchase failed (insufficient credits or max level)."));
		}
	}

	return false;
}

FText AOrbitalGameMode::GetObjectiveText() const
{
	return MissionComponent ? MissionComponent->GetObjectiveText() : FText::FromString(TEXT("No mission."));
}

FString AOrbitalGameMode::GetCurrentSectorName() const
{
	switch (CurrentSector)
	{
	case EOrbitalSectorId::Belt:
		return TEXT("Belt");
	case EOrbitalSectorId::Ruins:
		return TEXT("Ruins");
	default:
		return TEXT("Unknown");
	}
}

void AOrbitalGameMode::SetStatusMessage(const FString& Message, float DurationSeconds)
{
	LastStatusMessage = Message;
	StatusMessageTimer = DurationSeconds;
}

AOrbitalShipPawn* AOrbitalGameMode::GetPlayerShip() const
{
	return Cast<AOrbitalShipPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void AOrbitalGameMode::LoadCurrentSectorAndPlaceShip(bool bForceResetVelocity)
{
	if (!SectorManager)
	{
		return;
	}

	SectorManager->LoadSector(CurrentSector);

	if (AOrbitalShipPawn* Ship = GetPlayerShip())
	{
		Ship->SetActorLocation(SectorManager->GetSectorSpawnPoint(CurrentSector));

		if (bForceResetVelocity && Ship->ShipMesh)
		{
			Ship->ShipMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
			Ship->ShipMesh->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
		}
	}
}
