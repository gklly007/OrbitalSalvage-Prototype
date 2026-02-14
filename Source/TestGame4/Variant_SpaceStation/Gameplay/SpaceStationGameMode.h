// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SpaceStationGameMode.generated.h"

class AStationGrid;
class AStationModule;
class ACrewMember;
class UStationSystemsComponent;
class UStationNotificationSystem;

/**
 * Game Mode for space station management.
 * Manages station resources, modules, crew, and gameplay systems.
 */
UCLASS(abstract)
class ASpaceStationGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	/** Station Systems Component for power, atmosphere, etc. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Systems")
	UStationSystemsComponent* StationSystemsComponent;

	/** Notification system for gameplay events */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Systems")
	UStationNotificationSystem* NotificationSystem;

	/** Type of Station Grid to spawn */
	UPROPERTY(EditAnywhere, Category="Space Station")
	TSubclassOf<AStationGrid> StationGridClass;

	/** Pointer to the spawned Station Grid */
	AStationGrid* StationGrid;

	/** Registry of all placed modules */
	TArray<AStationModule*> AllModules;

	/** Registry of all crew members */
	TArray<ACrewMember*> AllCrew;

	/** Current power available */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Resources")
	int32 CurrentPower = 0;

	/** Current oxygen available */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Resources")
	int32 CurrentOxygen = 0;

	/** Current food supply */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Resources")
	int32 CurrentFood = 1000;

	/** Starting power */
	UPROPERTY(EditAnywhere, Category="Space Station")
	int32 StartingPower = 0;

	/** Starting oxygen */
	UPROPERTY(EditAnywhere, Category="Space Station")
	int32 StartingOxygen = 0;

	/** Starting food */
	UPROPERTY(EditAnywhere, Category="Space Station")
	int32 StartingFood = 1000;

	/** Currency for building */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Resources")
	int32 CurrentCredits = 0;

	/** Starting credits */
	UPROPERTY(EditAnywhere, Category="Space Station")
	int32 StartingCredits = 5000;

	/** Crew member class to spawn */
	UPROPERTY(EditAnywhere, Category="Crew")
	TSubclassOf<ACrewMember> CrewMemberClass;

	/** Available module types for the build menu */
	UPROPERTY(EditAnywhere, Category="Building")
	TArray<TSubclassOf<AStationModule>> AvailableModuleClasses;

public:

	/** Constructor */
	ASpaceStationGameMode();

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Tick for resource updates */
	virtual void Tick(float DeltaSeconds) override;

	/** Cleanup */
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:

	/** Registers a module with the game mode */
	UFUNCTION(BlueprintCallable, Category="Station")
	void RegisterModule(AStationModule* Module);

	/** Unregisters a module from the game mode */
	UFUNCTION(BlueprintCallable, Category="Station")
	void UnregisterModule(AStationModule* Module);

	/** Registers a crew member with the game mode */
	UFUNCTION(BlueprintCallable, Category="Station")
	void RegisterCrew(ACrewMember* Crew);

	/** Unregisters a crew member from the game mode */
	UFUNCTION(BlueprintCallable, Category="Station")
	void UnregisterCrew(ACrewMember* Crew);

	/** Checks if player can afford a module */
	UFUNCTION(BlueprintPure, Category="Station")
	bool CanAffordModule(int32 Cost) const;

	/** Deducts resources for module placement */
	UFUNCTION(BlueprintCallable, Category="Station")
	void PayForModule(int32 Cost);

	/** Returns the station grid */
	UFUNCTION(BlueprintPure, Category="Station")
	AStationGrid* GetStationGrid() const { return StationGrid; }

	/** Returns the station systems component */
	UFUNCTION(BlueprintPure, Category="Station")
	UStationSystemsComponent* GetStationSystems() const { return StationSystemsComponent; }

	/** Returns all modules */
	UFUNCTION(BlueprintPure, Category="Station")
	const TArray<AStationModule*>& GetAllModules() const { return AllModules; }

	/** Returns all crew */
	const TArray<ACrewMember*>& GetAllCrew() const { return AllCrew; }

	/** Returns current food supply */
	UFUNCTION(BlueprintPure, Category="Resources")
	int32 GetCurrentFood() const { return CurrentFood; }

	/** Returns current power */
	UFUNCTION(BlueprintPure, Category="Resources")
	int32 GetCurrentPower() const { return CurrentPower; }

	/** Returns current oxygen */
	UFUNCTION(BlueprintPure, Category="Resources")
	int32 GetCurrentOxygen() const { return CurrentOxygen; }

	/** Returns current credits */
	UFUNCTION(BlueprintPure, Category="Resources")
	int32 GetCurrentCredits() const { return CurrentCredits; }

	/** Add credits */
	UFUNCTION(BlueprintCallable, Category="Resources")
	void AddCredits(int32 Amount) { CurrentCredits += Amount; }

	/** Spawn a crew member at a location */
	UFUNCTION(BlueprintCallable, Category="Crew")
	ACrewMember* SpawnCrewMember(const FVector& SpawnLocation);

	/** Get available module classes for the build menu */
	UFUNCTION(BlueprintPure, Category="Building")
	const TArray<TSubclassOf<AStationModule>>& GetAvailableModules() const { return AvailableModuleClasses; }

	/** Get notification system */
	UFUNCTION(BlueprintPure, Category="Station")
	UStationNotificationSystem* GetNotificationSystem() const { return NotificationSystem; }

	/** Recalculate station systems (call after module changes) */
	UFUNCTION(BlueprintCallable, Category="Station")
	void RecalculateSystems();

protected:

	/** Creates and spawns the station grid */
	void CreateStationGrid();
};
