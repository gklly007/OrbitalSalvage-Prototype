// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrewAIController.h"
#include "CrewMember.h"
#include "CrewNeedsComponent.h"
#include "StationModule.h"
#include "SpaceStationGameMode.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"

ACrewAIController::ACrewAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACrewAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	CrewMember = Cast<ACrewMember>(InPawn);
}

void ACrewAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!CrewMember || !CrewMember->IsAlive())
	{
		if (CurrentState != ECrewAIState::Dead)
		{
			SetAIState(ECrewAIState::Dead);
		}
		return;
	}

	// Don't evaluate while following player commands
	if (bHasPlayerCommand)
		return;

	// Periodic evaluation
	EvaluationTimer += DeltaSeconds;
	if (EvaluationTimer >= EvaluationInterval)
	{
		EvaluationTimer = 0.0f;
		EvaluateNeeds();
	}

	// Handle idle timer
	if (CurrentState == ECrewAIState::Idle)
	{
		HandleIdle();
	}
}

void ACrewAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	bIsMoving = false;

	if (!CrewMember)
		return;

	if (Result.IsSuccess())
	{
		HandleArrival();
	}
	else
	{
		// Move failed - go back to idle and re-evaluate
		if (!bHasPlayerCommand)
		{
			SetAIState(ECrewAIState::Idle);
		}
	}
}

void ACrewAIController::SetAIState(ECrewAIState NewState)
{
	if (CurrentState == NewState)
		return;

	CurrentState = NewState;
	BP_AIStateChanged(NewState);
}

void ACrewAIController::CommandMoveToLocation(const FVector& Location)
{
	if (!CrewMember)
		return;

	bHasPlayerCommand = true;
	CrewMember->StopInteraction();
	SetAIState(ECrewAIState::FollowingCommand);
	MoveToLocation(Location, 50.0f);
	bIsMoving = true;
}

void ACrewAIController::CommandMoveToModule(AStationModule* Module)
{
	if (!CrewMember || !Module)
		return;

	bHasPlayerCommand = true;
	CrewMember->StopInteraction();
	CrewMember->TargetModule = Module;
	SetAIState(ECrewAIState::FollowingCommand);
	MoveToLocation(Module->GetActorLocation(), 100.0f);
	bIsMoving = true;
}

void ACrewAIController::EvaluateNeeds()
{
	if (!CrewMember)
		return;

	UCrewNeedsComponent* Needs = CrewMember->GetNeedsComponent();
	if (!Needs)
		return;

	// Don't interrupt interaction unless something more urgent comes up
	if (CurrentState == ECrewAIState::Interacting)
	{
		// Only interrupt if a higher-priority need becomes critical
		if (CrewMember->CurrentInteraction == EModuleInteractionType::Sleep && Needs->IsNeedCritical(ECrewNeedType::Oxygen))
		{
			CrewMember->StopInteraction();
			SeekModuleForNeed(ECrewNeedType::Oxygen);
			return;
		}
		if (CrewMember->CurrentInteraction == EModuleInteractionType::Food && Needs->IsNeedCritical(ECrewNeedType::Oxygen))
		{
			CrewMember->StopInteraction();
			SeekModuleForNeed(ECrewNeedType::Oxygen);
			return;
		}
		// Otherwise, let interaction complete
		return;
	}

	// Already moving to fulfill a need - don't interrupt unless critical
	if (bIsMoving && CurrentState != ECrewAIState::Idle)
	{
		return;
	}

	// Priority evaluation: Oxygen > Food > Sleep
	if (Needs->IsNeedLow(ECrewNeedType::Oxygen))
	{
		SeekModuleForNeed(ECrewNeedType::Oxygen);
		return;
	}

	if (Needs->IsNeedLow(ECrewNeedType::Food))
	{
		SeekModuleForNeed(ECrewNeedType::Food);
		return;
	}

	if (Needs->IsNeedLow(ECrewNeedType::Sleep))
	{
		SeekModuleForNeed(ECrewNeedType::Sleep);
		return;
	}

	// Nothing urgent - idle
	if (CurrentState != ECrewAIState::Idle)
	{
		SetAIState(ECrewAIState::Idle);
		IdleTimer = 0.0f;
	}
}

void ACrewAIController::ExecuteState()
{
	// This is called after state transitions if we need immediate action
	// Currently handled by EvaluateNeeds and OnMoveCompleted
}

void ACrewAIController::SeekModuleForNeed(ECrewNeedType NeedType)
{
	if (!CrewMember)
		return;

	AStationModule* Target = FindNearestModuleForNeed(NeedType);
	if (!Target)
	{
		// No module available - stay idle
		SetAIState(ECrewAIState::Idle);
		return;
	}

	// Set appropriate state
	switch (NeedType)
	{
	case ECrewNeedType::Oxygen:
		SetAIState(ECrewAIState::SeekingOxygen);
		break;
	case ECrewNeedType::Food:
		SetAIState(ECrewAIState::SeekingFood);
		break;
	case ECrewNeedType::Sleep:
		SetAIState(ECrewAIState::SeekingSleep);
		break;
	default:
		break;
	}

	CrewMember->MoveToModule(Target);
	bIsMoving = true;
}

void ACrewAIController::HandleArrival()
{
	if (!CrewMember)
		return;

	// Determine interaction type based on current state
	EModuleInteractionType InteractionType = EModuleInteractionType::None;

	switch (CurrentState)
	{
	case ECrewAIState::SeekingOxygen:
		InteractionType = EModuleInteractionType::Oxygen;
		break;
	case ECrewAIState::SeekingFood:
		InteractionType = EModuleInteractionType::Food;
		break;
	case ECrewAIState::SeekingSleep:
		InteractionType = EModuleInteractionType::Sleep;
		break;
	case ECrewAIState::FollowingCommand:
		// Player command complete - return to autonomous behavior
		bHasPlayerCommand = false;
		SetAIState(ECrewAIState::Idle);
		return;
	default:
		SetAIState(ECrewAIState::Idle);
		return;
	}

	// Start interacting
	if (CrewMember->TargetModule && InteractionType != EModuleInteractionType::None)
	{
		SetAIState(ECrewAIState::Interacting);
		CrewMember->StartInteraction(CrewMember->TargetModule, InteractionType);
	}
	else
	{
		SetAIState(ECrewAIState::Idle);
	}
}

void ACrewAIController::HandleIdle()
{
	// Currently just wait - could add wander behavior later
	IdleTimer += GetWorld()->GetDeltaSeconds();

	// After idle duration, re-evaluate immediately
	if (IdleTimer >= IdleDuration)
	{
		IdleTimer = 0.0f;
		EvaluateNeeds();
	}
}

AStationModule* ACrewAIController::FindNearestLifeSupport() const
{
	return FindNearestModuleForNeed(ECrewNeedType::Oxygen);
}

AStationModule* ACrewAIController::FindNearestFoodSource() const
{
	return FindNearestModuleForNeed(ECrewNeedType::Food);
}

AStationModule* ACrewAIController::FindNearestQuarters() const
{
	return FindNearestModuleForNeed(ECrewNeedType::Sleep);
}

AStationModule* ACrewAIController::FindNearestModuleForNeed(ECrewNeedType NeedType) const
{
	if (!CrewMember)
		return nullptr;

	TArray<AStationModule*> Modules = GetAllModules();
	AStationModule* NearestModule = nullptr;
	float NearestDistance = MAX_FLT;
	FVector CrewLocation = CrewMember->GetActorLocation();

	for (AStationModule* Module : Modules)
	{
		if (!Module || !Module->bIsPlaced || !Module->bIsPowered)
			continue;

		bool bMatchesNeed = false;
		switch (NeedType)
		{
		case ECrewNeedType::Oxygen:
			// Life support modules generate oxygen
			bMatchesNeed = (Module->ModuleType == EStationModuleType::LifeSupport && Module->bHasAtmosphere);
			break;
		case ECrewNeedType::Food:
			// Mess hall or modules with food capability
			bMatchesNeed = (Module->ModuleType == EStationModuleType::MessHall && Module->bHasAtmosphere);
			break;
		case ECrewNeedType::Sleep:
			// Quarters for sleeping
			bMatchesNeed = (Module->ModuleType == EStationModuleType::Quarters && Module->bHasAtmosphere);
			break;
		default:
			break;
		}

		if (bMatchesNeed)
		{
			float Distance = FVector::Dist2D(CrewLocation, Module->GetActorLocation());
			if (Distance < NearestDistance)
			{
				NearestDistance = Distance;
				NearestModule = Module;
			}
		}
	}

	return NearestModule;
}

TArray<AStationModule*> ACrewAIController::GetAllModules() const
{
	ASpaceStationGameMode* GM = Cast<ASpaceStationGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM)
	{
		return GM->GetAllModules();
	}
	return TArray<AStationModule*>();
}
