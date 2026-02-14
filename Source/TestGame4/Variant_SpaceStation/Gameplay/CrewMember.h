// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CrewNeedsComponent.h"
#include "CrewMember.generated.h"

class AStationModule;
class UCrewNeedsComponent;

/**
 * Enum for crew member module interaction types.
 */
UENUM(BlueprintType)
enum class EModuleInteractionType : uint8
{
	None,
	Oxygen,       // At life support - replenish oxygen
	Food,         // At mess hall - replenish food
	Sleep,        // At quarters - replenish sleep
	Work          // At workstation - generic work
};

/**
 * A crew member that lives on the space station.
 * Has survival needs (oxygen, food, sleep) and can be selected and commanded.
 * AI-controlled to automatically seek modules to fulfill needs.
 */
UCLASS()
class ACrewMember : public ACharacter
{
	GENERATED_BODY()

protected:

	/** Crew needs component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UCrewNeedsComponent* NeedsComponent;

public:

	ACrewMember();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// Identity

	/** Display name of this crew member */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crew")
	FText CrewName = FText::FromString("Crew Member");

	/** Auto-generate a random name on spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crew")
	bool bAutoGenerateName = true;

	/** Generate a random crew name */
	UFUNCTION(BlueprintCallable, Category="Crew")
	void GenerateRandomName();

	// Selection

	/** Is this crew member currently selected by the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Selection")
	bool bIsSelected = false;

	// Module Tracking

	/** The module this crew member is currently inside */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Module")
	AStationModule* CurrentModule;

	/** The module this crew member is moving towards */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Module")
	AStationModule* TargetModule;

	/** What type of interaction is happening at the current module */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Module")
	EModuleInteractionType CurrentInteraction = EModuleInteractionType::None;

	/** Is the crew member currently interacting with a module */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Module")
	bool bIsInteracting = false;

	// Movement

	/** Movement speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float MoveSpeed = 300.0f;

public:

	// Selection

	/** Set selected state */
	UFUNCTION(BlueprintCallable, Category="Selection")
	void SetSelected(bool bSelected);

	// Movement Commands

	/** Move to a world location */
	UFUNCTION(BlueprintCallable, Category="Movement")
	void MoveToLocation(const FVector& Location);

	/** Move to a specific module */
	UFUNCTION(BlueprintCallable, Category="Movement")
	void MoveToModule(AStationModule* Module);

	/** Stop movement and interaction */
	UFUNCTION(BlueprintCallable, Category="Movement")
	void StopMovement();

	// Module Interaction

	/** Start interacting with a module */
	UFUNCTION(BlueprintCallable, Category="Module")
	void StartInteraction(AStationModule* Module, EModuleInteractionType InteractionType);

	/** Stop current interaction */
	UFUNCTION(BlueprintCallable, Category="Module")
	void StopInteraction();

	/** Check if crew member has arrived at target module */
	UFUNCTION(BlueprintPure, Category="Module")
	bool HasArrivedAtTarget() const;

	// Needs Access

	/** Get the needs component */
	UFUNCTION(BlueprintPure, Category="Needs")
	UCrewNeedsComponent* GetNeedsComponent() const { return NeedsComponent; }

	/** Check if crew member is alive */
	UFUNCTION(BlueprintPure, Category="Needs")
	bool IsAlive() const;

	// Blueprint Events

	/** Called when crew member is selected/deselected */
	UFUNCTION(BlueprintImplementableEvent, Category="Selection")
	void BP_SelectionChanged(bool bSelected);

	/** Called when crew member arrives at target module */
	UFUNCTION(BlueprintImplementableEvent, Category="Movement")
	void BP_ArrivedAtModule(AStationModule* Module);

	/** Called when interaction starts */
	UFUNCTION(BlueprintImplementableEvent, Category="Module")
	void BP_InteractionStarted(EModuleInteractionType InteractionType);

	/** Called when interaction ends */
	UFUNCTION(BlueprintImplementableEvent, Category="Module")
	void BP_InteractionEnded();

private:

	/** Update current module based on position */
	void UpdateCurrentModule();

	/** Update atmosphere state from current module */
	void UpdateAtmosphereState();

	/** Tick interaction replenishment */
	void TickInteraction(float DeltaTime);

	/** Arrival distance threshold */
	float ArrivalDistance = 100.0f;
};
