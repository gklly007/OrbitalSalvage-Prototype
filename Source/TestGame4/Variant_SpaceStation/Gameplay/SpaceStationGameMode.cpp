// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpaceStationGameMode.h"
#include "StationGrid.h"
#include "StationModule.h"
#include "StationSystemsComponent.h"
#include "StationNotificationSystem.h"
#include "CrewMember.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ASpaceStationGameMode::ASpaceStationGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create station systems component
	StationSystemsComponent = CreateDefaultSubobject<UStationSystemsComponent>(TEXT("StationSystemsComponent"));

	// Create notification system
	NotificationSystem = CreateDefaultSubobject<UStationNotificationSystem>(TEXT("NotificationSystem"));
}

void ASpaceStationGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Initialize resources
	CurrentPower = StartingPower;
	CurrentOxygen = StartingOxygen;
	CurrentFood = StartingFood;
	CurrentCredits = StartingCredits;

	// Create the station grid
	CreateStationGrid();
}

void ASpaceStationGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Tick station systems
	if (StationSystemsComponent)
	{
		StationSystemsComponent->TickSystems(DeltaSeconds);

		// Sync resource values from systems component
		CurrentPower = StationSystemsComponent->GetNetPower();
		CurrentOxygen = StationSystemsComponent->GetNetOxygen();
	}
}

void ASpaceStationGameMode::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASpaceStationGameMode::RegisterModule(AStationModule* Module)
{
	if (Module && !AllModules.Contains(Module))
	{
		AllModules.Add(Module);
		RecalculateSystems();

		if (NotificationSystem)
		{
			NotificationSystem->AddNotification(
				FText::FromString(FString::Printf(TEXT("%s placed"), *Module->ModuleName.ToString())),
				ENotificationPriority::Info, 3.0f);
		}
	}
}

void ASpaceStationGameMode::UnregisterModule(AStationModule* Module)
{
	if (Module)
	{
		AllModules.Remove(Module);
		RecalculateSystems();
	}
}

void ASpaceStationGameMode::RegisterCrew(ACrewMember* Crew)
{
	if (Crew && !AllCrew.Contains(Crew))
	{
		AllCrew.Add(Crew);

		if (NotificationSystem)
		{
			NotificationSystem->AddNotification(
				FText::FromString(FString::Printf(TEXT("%s joined the crew"), *Crew->CrewName.ToString())),
				ENotificationPriority::Success, 4.0f);
		}
	}
}

void ASpaceStationGameMode::UnregisterCrew(ACrewMember* Crew)
{
	if (Crew)
	{
		AllCrew.Remove(Crew);
	}
}

bool ASpaceStationGameMode::CanAffordModule(int32 Cost) const
{
	return CurrentCredits >= Cost;
}

void ASpaceStationGameMode::PayForModule(int32 Cost)
{
	CurrentCredits = FMath::Max(0, CurrentCredits - Cost);
}

ACrewMember* ASpaceStationGameMode::SpawnCrewMember(const FVector& SpawnLocation)
{
	TSubclassOf<ACrewMember> ClassToSpawn = CrewMemberClass;
	if (!ClassToSpawn)
		ClassToSpawn = ACrewMember::StaticClass();


	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ACrewMember* NewCrew = GetWorld()->SpawnActor<ACrewMember>(ClassToSpawn, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	return NewCrew; // RegisterCrew is called from CrewMember::BeginPlay
}

void ASpaceStationGameMode::RecalculateSystems()
{
	if (StationSystemsComponent)
	{
		StationSystemsComponent->RecalculateResources();
		StationSystemsComponent->UpdatePowerDistribution();
		StationSystemsComponent->PropagateAtmosphere();
	}
}

void ASpaceStationGameMode::CreateStationGrid()
{
	if (StationGrid)
		return;

	if (StationGridClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		StationGrid = GetWorld()->SpawnActor<AStationGrid>(StationGridClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	}
	else
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		StationGrid = GetWorld()->SpawnActor<AStationGrid>(AStationGrid::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	}
}
