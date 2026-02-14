// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CrewNeedsComponent.h"
#include "CrewAIController.generated.h"

class ACrewMember;
class AStationModule;

/**
 * Enum for crew AI states.
 */
UENUM(BlueprintType)
enum class ECrewAIState : uint8
{
	Idle,             // Wandering or waiting
	SeekingOxygen,    // Moving to life support
	SeekingFood,      // Moving to mess hall/food storage
	SeekingSleep,     // Moving to quarters
	Interacting,      // At a module, replenishing
	FollowingCommand, // Player issued a move command
	Dead              // Crew member has died
};

/**
 * AI Controller for crew members.
 * Uses a simple state machine driven by crew needs.
 * Priority: Oxygen > Food > Sleep > Idle
 */
UCLASS(abstract)
class ACrewAIController : public AAIController
{
	GENERATED_BODY()

public:

	ACrewAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

	/** Called when a move request completes */
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	// AI State

	/** Current AI state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	ECrewAIState CurrentState = ECrewAIState::Idle;

	/** How often the AI re-evaluates its state (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float EvaluationInterval = 1.0f;

	/** How long to idle before re-evaluating (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float IdleDuration = 3.0f;

public:

	/** Force the AI into a specific state (e.g., player command) */
	UFUNCTION(BlueprintCallable, Category="AI")
	void SetAIState(ECrewAIState NewState);

	/** Command this crew to move to a location (player issued) */
	UFUNCTION(BlueprintCallable, Category="AI")
	void CommandMoveToLocation(const FVector& Location);

	/** Command this crew to move to a module (player issued) */
	UFUNCTION(BlueprintCallable, Category="AI")
	void CommandMoveToModule(AStationModule* Module);

	/** Find the nearest placed module that has oxygen generation */
	UFUNCTION(BlueprintPure, Category="AI")
	AStationModule* FindNearestLifeSupport() const;

	/** Find the nearest placed module that has crew capacity (for food) */
	UFUNCTION(BlueprintPure, Category="AI")
	AStationModule* FindNearestFoodSource() const;

	/** Find the nearest placed module with sleep capacity */
	UFUNCTION(BlueprintPure, Category="AI")
	AStationModule* FindNearestQuarters() const;

	/** Find the nearest module of a given type based on need */
	UFUNCTION(BlueprintPure, Category="AI")
	AStationModule* FindNearestModuleForNeed(ECrewNeedType NeedType) const;

	// Blueprint Events

	/** Called when AI state changes */
	UFUNCTION(BlueprintImplementableEvent, Category="AI")
	void BP_AIStateChanged(ECrewAIState NewState);

private:

	/** Evaluate needs and decide what to do */
	void EvaluateNeeds();

	/** Execute current state behavior */
	void ExecuteState();

	/** Handle seeking a module for a need */
	void SeekModuleForNeed(ECrewNeedType NeedType);

	/** Handle arrival at target module */
	void HandleArrival();

	/** Handle idle behavior */
	void HandleIdle();

	/** Cached crew member reference */
	ACrewMember* CrewMember = nullptr;

	/** Timer for AI evaluation */
	float EvaluationTimer = 0.0f;

	/** Timer for idle state */
	float IdleTimer = 0.0f;

	/** Is the crew member currently moving to a target */
	bool bIsMoving = false;

	/** Has the player issued a manual command */
	bool bHasPlayerCommand = false;

	/** Get all modules from game mode */
	TArray<AStationModule*> GetAllModules() const;
};
