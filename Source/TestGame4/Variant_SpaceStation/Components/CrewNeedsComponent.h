// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CrewNeedsComponent.generated.h"

/**
 * Enum for the different crew need types.
 */
UENUM(BlueprintType)
enum class ECrewNeedType : uint8
{
	Oxygen,
	Food,
	Sleep,
	Health
};

/**
 * Manages individual crew member survival needs.
 * Tracks oxygen, food, sleep, and health values that deplete over time
 * and can be replenished at appropriate station modules.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UCrewNeedsComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UCrewNeedsComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Need Values (0-100)

	/** Current oxygen level */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Needs")
	float Oxygen = 100.0f;

	/** Current food level */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Needs")
	float Food = 100.0f;

	/** Current sleep level */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Needs")
	float Sleep = 100.0f;

	/** Current health level */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Needs")
	float Health = 100.0f;

	// Depletion Rates (per second)

	/** Oxygen depletion rate per second */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Needs|Rates")
	float OxygenDepletionRate = 2.0f;

	/** Food depletion rate per second */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Needs|Rates")
	float FoodDepletionRate = 0.5f;

	/** Sleep depletion rate per second */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Needs|Rates")
	float SleepDepletionRate = 0.3f;

	/** Health regeneration rate per second (when all needs met) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Needs|Rates")
	float HealthRegenRate = 1.0f;

	/** Health depletion rate when oxygen is critical */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Needs|Rates")
	float HealthDepletionFromOxygen = 5.0f;

	/** Health depletion rate when food is critical */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Needs|Rates")
	float HealthDepletionFromFood = 2.0f;

	// Replenishment Rates (per second, while interacting with module)

	/** Oxygen replenishment rate at life support */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Needs|Replenish")
	float OxygenReplenishRate = 10.0f;

	/** Food replenishment rate at mess hall */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Needs|Replenish")
	float FoodReplenishRate = 5.0f;

	/** Sleep replenishment rate at quarters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Needs|Replenish")
	float SleepReplenishRate = 3.0f;

	// Thresholds

	/** Below this value, need is considered critical */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Needs|Thresholds")
	float CriticalThreshold = 20.0f;

	/** Below this value, need is considered low */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Needs|Thresholds")
	float LowThreshold = 40.0f;

	// State

	/** Is crew member currently in a module with atmosphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Needs")
	bool bInAtmosphere = false;

	/** Is crew member alive */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Needs")
	bool bIsAlive = true;

public:

	/** Get the value of a specific need */
	UFUNCTION(BlueprintPure, Category="Needs")
	float GetNeedValue(ECrewNeedType NeedType) const;

	/** Check if a specific need is critical */
	UFUNCTION(BlueprintPure, Category="Needs")
	bool IsNeedCritical(ECrewNeedType NeedType) const;

	/** Check if a specific need is low */
	UFUNCTION(BlueprintPure, Category="Needs")
	bool IsNeedLow(ECrewNeedType NeedType) const;

	/** Get the most urgent need (lowest percentage that's below LowThreshold) */
	UFUNCTION(BlueprintPure, Category="Needs")
	ECrewNeedType GetMostUrgentNeed() const;

	/** Check if any need is critical */
	UFUNCTION(BlueprintPure, Category="Needs")
	bool HasAnyCriticalNeed() const;

	/** Replenish oxygen (called while at life support module) */
	UFUNCTION(BlueprintCallable, Category="Needs")
	void ReplenishOxygen(float DeltaTime);

	/** Replenish food (called while at mess hall module) */
	UFUNCTION(BlueprintCallable, Category="Needs")
	void ReplenishFood(float DeltaTime);

	/** Replenish sleep (called while at quarters module) */
	UFUNCTION(BlueprintCallable, Category="Needs")
	void ReplenishSleep(float DeltaTime);

	/** Set atmosphere state */
	UFUNCTION(BlueprintCallable, Category="Needs")
	void SetInAtmosphere(bool bHasAtmosphere);

	// Blueprint Events

	/** Called when a need becomes critical (<20%) */
	UFUNCTION(BlueprintImplementableEvent, Category="Needs")
	void BP_NeedCritical(ECrewNeedType NeedType);

	/** Called when crew member dies */
	UFUNCTION(BlueprintImplementableEvent, Category="Needs")
	void BP_CrewDied();

private:

	/** Deplete needs over time */
	void TickNeeds(float DeltaTime);

	/** Update health based on other needs */
	void TickHealth(float DeltaTime);

	/** Track previous critical states to fire events only on change */
	bool bWasOxygenCritical = false;
	bool bWasFoodCritical = false;
	bool bWasSleepCritical = false;
};
