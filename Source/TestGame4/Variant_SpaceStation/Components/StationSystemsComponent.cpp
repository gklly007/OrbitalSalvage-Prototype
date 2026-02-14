// Copyright Epic Games, Inc. All Rights Reserved.

#include "StationSystemsComponent.h"
#include "StationModule.h"
#include "StationGrid.h"
#include "SpaceStationGameMode.h"
#include "Kismet/GameplayStatics.h"

UStationSystemsComponent::UStationSystemsComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // Ticked manually from GameMode
}

void UStationSystemsComponent::TickSystems(float DeltaSeconds)
{
	SystemUpdateTimer += DeltaSeconds;
	if (SystemUpdateTimer >= SystemUpdateInterval)
	{
		SystemUpdateTimer = 0.0f;
		RecalculateResources();
		UpdatePowerDistribution();
		PropagateAtmosphere();
	}
}

void UStationSystemsComponent::RecalculateResources()
{
	int32 OldPowerGen = TotalPowerGeneration;
	int32 OldPowerCon = TotalPowerConsumption;
	int32 OldOxygenGen = TotalOxygenGeneration;
	int32 OldOxygenCon = TotalOxygenConsumption;

	TotalPowerGeneration = 0;
	TotalPowerConsumption = 0;
	TotalOxygenGeneration = 0;
	TotalOxygenConsumption = 0;

	TArray<AStationModule*> Modules = GetAllModules();
	for (AStationModule* Module : Modules)
	{
		if (!Module || !Module->bIsPlaced)
			continue;

		TotalPowerGeneration += Module->PowerGeneration;
		TotalPowerConsumption += Module->PowerConsumption;
		TotalOxygenGeneration += Module->OxygenGeneration;
		TotalOxygenConsumption += Module->OxygenConsumption;
	}

	bool bOldPowerState = bHasSufficientPower;
	bool bOldOxygenState = bHasSufficientOxygen;

	bHasSufficientPower = TotalPowerGeneration >= TotalPowerConsumption;
	bHasSufficientOxygen = TotalOxygenGeneration >= TotalOxygenConsumption;

	// Fire events if state changed
	if (bOldPowerState != bHasSufficientPower)
	{
		OnPowerStateChanged.Broadcast();
	}
	if (bOldOxygenState != bHasSufficientOxygen)
	{
		OnOxygenStateChanged.Broadcast();
	}
}

void UStationSystemsComponent::UpdatePowerDistribution()
{
	TArray<AStationModule*> Modules = GetAllModules();

	for (AStationModule* Module : Modules)
	{
		if (!Module || !Module->bIsPlaced)
			continue;

		// Modules that don't require power are always powered
		if (!Module->bRequiresPower)
		{
			if (!Module->bIsPowered)
			{
				Module->SetPoweredState(true);
			}
			continue;
		}

		// If station has sufficient power, all modules are powered
		// If not, nothing is powered (simple global pool model)
		bool bShouldBePowered = bHasSufficientPower;
		if (Module->bIsPowered != bShouldBePowered)
		{
			Module->SetPoweredState(bShouldBePowered);
		}
	}
}

void UStationSystemsComponent::PropagateAtmosphere()
{
	TArray<AStationModule*> Modules = GetAllModules();

	// First, clear all atmosphere flags
	for (AStationModule* Module : Modules)
	{
		if (Module && Module->bIsPlaced)
		{
			Module->bHasAtmosphere = false;
		}
	}

	// Find all powered life support modules (OxygenGeneration > 0)
	// and flood-fill atmosphere from them
	TSet<AStationModule*> VisitedModules;

	for (AStationModule* Module : Modules)
	{
		if (!Module || !Module->bIsPlaced)
			continue;

		// Life support: generates oxygen and is currently powered
		if (Module->OxygenGeneration > 0 && Module->bIsPowered)
		{
			FloodFillAtmosphere(Module, VisitedModules);
		}
	}
}

void UStationSystemsComponent::FloodFillAtmosphere(AStationModule* StartModule, TSet<AStationModule*>& VisitedModules)
{
	if (!StartModule || VisitedModules.Contains(StartModule))
		return;

	// Use a queue for BFS flood fill
	TQueue<AStationModule*> Queue;
	Queue.Enqueue(StartModule);
	VisitedModules.Add(StartModule);

	while (!Queue.IsEmpty())
	{
		AStationModule* Current;
		Queue.Dequeue(Current);

		// Set atmosphere on this module
		Current->bHasAtmosphere = true;

		// Spread to connected modules
		for (AStationModule* Connected : Current->ConnectedModules)
		{
			if (Connected && Connected->bIsPlaced && !VisitedModules.Contains(Connected))
			{
				VisitedModules.Add(Connected);
				Queue.Enqueue(Connected);
			}
		}
	}
}

TArray<AStationModule*> UStationSystemsComponent::GetAllModules() const
{
	ASpaceStationGameMode* GM = Cast<ASpaceStationGameMode>(GetOwner());
	if (GM)
	{
		return GM->GetAllModules();
	}
	return TArray<AStationModule*>();
}

AStationGrid* UStationSystemsComponent::GetStationGrid() const
{
	ASpaceStationGameMode* GM = Cast<ASpaceStationGameMode>(GetOwner());
	if (GM)
	{
		return GM->GetStationGrid();
	}
	return nullptr;
}
