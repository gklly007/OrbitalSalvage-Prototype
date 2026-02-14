// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrewMember.h"
#include "CrewNeedsComponent.h"
#include "StationModule.h"
#include "StationGrid.h"
#include "SpaceStationGameMode.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ACrewMember::ACrewMember()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create needs component
	NeedsComponent = CreateDefaultSubobject<UCrewNeedsComponent>(TEXT("NeedsComponent"));

	// Configure movement
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = MoveSpeed;
		MoveComp->bConstrainToPlane = true;
		MoveComp->SetPlaneConstraintNormal(FVector::UpVector);
	}
}

void ACrewMember::BeginPlay()
{
	Super::BeginPlay();

	// Auto-generate name
	if (bAutoGenerateName)
	{
		GenerateRandomName();
	}

	// Register with game mode
	if (ASpaceStationGameMode* GM = GetWorld()->GetAuthGameMode<ASpaceStationGameMode>())
	{
		GM->RegisterCrew(this);
	}
}

void ACrewMember::GenerateRandomName()
{
	static const TArray<FString> FirstNames = {
		TEXT("Alex"), TEXT("Jordan"), TEXT("Morgan"), TEXT("Casey"),
		TEXT("Riley"), TEXT("Quinn"), TEXT("Avery"), TEXT("Dakota"),
		TEXT("Taylor"), TEXT("Cameron"), TEXT("Skyler"), TEXT("Reese"),
		TEXT("Nova"), TEXT("Orion"), TEXT("Atlas"), TEXT("Phoenix"),
		TEXT("Kai"), TEXT("Sage"), TEXT("River"), TEXT("Aspen"),
		TEXT("Jax"), TEXT("Zara"), TEXT("Yuri"), TEXT("Mika"),
		TEXT("Soren"), TEXT("Lyra"), TEXT("Cyrus"), TEXT("Vesper"),
		TEXT("Nico"), TEXT("Wren"), TEXT("Ash"), TEXT("Sol")
	};

	static const TArray<FString> LastNames = {
		TEXT("Chen"), TEXT("Kowalski"), TEXT("Okafor"), TEXT("Santos"),
		TEXT("Volkov"), TEXT("Nakamura"), TEXT("Petrov"), TEXT("Singh"),
		TEXT("Mueller"), TEXT("Park"), TEXT("Reyes"), TEXT("Andersen"),
		TEXT("Kofi"), TEXT("Yamamoto"), TEXT("Ivanova"), TEXT("Novak"),
		TEXT("Torres"), TEXT("Kim"), TEXT("Osei"), TEXT("Eriksson"),
		TEXT("Abe"), TEXT("Costa"), TEXT("Bergman"), TEXT("Tanaka"),
		TEXT("Shah"), TEXT("Larsen"), TEXT("Moreau"), TEXT("Gupta"),
		TEXT("Varga"), TEXT("Zhao"), TEXT("Okoro"), TEXT("Lund")
	};

	int32 FirstIdx = FMath::RandRange(0, FirstNames.Num() - 1);
	int32 LastIdx = FMath::RandRange(0, LastNames.Num() - 1);

	CrewName = FText::FromString(FString::Printf(TEXT("%s %s"), *FirstNames[FirstIdx], *LastNames[LastIdx]));
}

void ACrewMember::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!IsAlive())
		return;

	UpdateCurrentModule();
	UpdateAtmosphereState();

	// Check if we've arrived at target
	if (TargetModule && HasArrivedAtTarget() && !bIsInteracting)
	{
		BP_ArrivedAtModule(TargetModule);
	}

	// Tick interaction
	if (bIsInteracting)
	{
		TickInteraction(DeltaSeconds);
	}
}

void ACrewMember::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;
	BP_SelectionChanged(bSelected);
}

void ACrewMember::MoveToLocation(const FVector& Location)
{
	StopInteraction();
	TargetModule = nullptr;

	// Use AI MoveTo if we have an AI controller
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->MoveToLocation(Location);
	}
}

void ACrewMember::MoveToModule(AStationModule* Module)
{
	if (!Module)
		return;

	StopInteraction();
	TargetModule = Module;

	// Move to module's world location
	FVector TargetLocation = Module->GetActorLocation();

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->MoveToLocation(TargetLocation, ArrivalDistance);
	}
}

void ACrewMember::StopMovement()
{
	StopInteraction();
	TargetModule = nullptr;

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->StopMovement();
	}
}

void ACrewMember::StartInteraction(AStationModule* Module, EModuleInteractionType InteractionType)
{
	if (!Module || InteractionType == EModuleInteractionType::None)
		return;

	CurrentInteraction = InteractionType;
	bIsInteracting = true;
	BP_InteractionStarted(InteractionType);
}

void ACrewMember::StopInteraction()
{
	if (bIsInteracting)
	{
		bIsInteracting = false;
		CurrentInteraction = EModuleInteractionType::None;
		BP_InteractionEnded();
	}
}

bool ACrewMember::HasArrivedAtTarget() const
{
	if (!TargetModule)
		return false;

	float Distance = FVector::Dist2D(GetActorLocation(), TargetModule->GetActorLocation());
	return Distance <= ArrivalDistance;
}

bool ACrewMember::IsAlive() const
{
	return NeedsComponent && NeedsComponent->bIsAlive;
}

void ACrewMember::UpdateCurrentModule()
{
	ASpaceStationGameMode* GM = GetWorld()->GetAuthGameMode<ASpaceStationGameMode>();
	if (!GM)
		return;

	AStationGrid* Grid = GM->GetStationGrid();
	if (!Grid)
		return;

	// Find which module we're standing in
	FIntPoint GridCoord = Grid->WorldToGrid(GetActorLocation());
	AStationModule* ModuleAtPos = Grid->GetModuleAt(GridCoord);
	CurrentModule = ModuleAtPos;
}

void ACrewMember::UpdateAtmosphereState()
{
	if (!NeedsComponent)
		return;

	// Crew has atmosphere if they're inside a module that has atmosphere
	bool bHasAtmo = (CurrentModule != nullptr && CurrentModule->bHasAtmosphere);
	NeedsComponent->SetInAtmosphere(bHasAtmo);
}

void ACrewMember::TickInteraction(float DeltaTime)
{
	if (!NeedsComponent || !bIsInteracting)
		return;

	// Check if we're still at the module
	if (TargetModule && !HasArrivedAtTarget())
	{
		StopInteraction();
		return;
	}

	// Replenish based on interaction type
	switch (CurrentInteraction)
	{
	case EModuleInteractionType::Oxygen:
		NeedsComponent->ReplenishOxygen(DeltaTime);
		// Stop when full
		if (NeedsComponent->Oxygen >= 99.0f)
		{
			StopInteraction();
		}
		break;

	case EModuleInteractionType::Food:
		NeedsComponent->ReplenishFood(DeltaTime);
		if (NeedsComponent->Food >= 99.0f)
		{
			StopInteraction();
		}
		break;

	case EModuleInteractionType::Sleep:
		NeedsComponent->ReplenishSleep(DeltaTime);
		if (NeedsComponent->Sleep >= 99.0f)
		{
			StopInteraction();
		}
		break;

	default:
		break;
	}
}
