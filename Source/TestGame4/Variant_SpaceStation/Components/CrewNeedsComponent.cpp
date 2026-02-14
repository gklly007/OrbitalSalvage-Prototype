// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrewNeedsComponent.h"

UCrewNeedsComponent::UCrewNeedsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCrewNeedsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsAlive)
		return;

	TickNeeds(DeltaTime);
	TickHealth(DeltaTime);
}

void UCrewNeedsComponent::TickNeeds(float DeltaTime)
{
	// Oxygen depletes faster when not in atmosphere
	if (!bInAtmosphere)
	{
		Oxygen = FMath::Max(0.0f, Oxygen - OxygenDepletionRate * DeltaTime);
	}
	else
	{
		// Slowly deplete even in atmosphere (need life support to fully sustain)
		Oxygen = FMath::Max(0.0f, Oxygen - OxygenDepletionRate * 0.25f * DeltaTime);
	}

	// Food always depletes
	Food = FMath::Max(0.0f, Food - FoodDepletionRate * DeltaTime);

	// Sleep always depletes
	Sleep = FMath::Max(0.0f, Sleep - SleepDepletionRate * DeltaTime);

	// Check for critical transitions and fire events
	bool bOxygenCritical = IsNeedCritical(ECrewNeedType::Oxygen);
	bool bFoodCritical = IsNeedCritical(ECrewNeedType::Food);
	bool bSleepCritical = IsNeedCritical(ECrewNeedType::Sleep);

	if (bOxygenCritical && !bWasOxygenCritical)
	{
		BP_NeedCritical(ECrewNeedType::Oxygen);
	}
	if (bFoodCritical && !bWasFoodCritical)
	{
		BP_NeedCritical(ECrewNeedType::Food);
	}
	if (bSleepCritical && !bWasSleepCritical)
	{
		BP_NeedCritical(ECrewNeedType::Sleep);
	}

	bWasOxygenCritical = bOxygenCritical;
	bWasFoodCritical = bFoodCritical;
	bWasSleepCritical = bSleepCritical;
}

void UCrewNeedsComponent::TickHealth(float DeltaTime)
{
	// Health depletes when critical needs aren't met
	if (IsNeedCritical(ECrewNeedType::Oxygen))
	{
		Health = FMath::Max(0.0f, Health - HealthDepletionFromOxygen * DeltaTime);
	}
	if (IsNeedCritical(ECrewNeedType::Food))
	{
		Health = FMath::Max(0.0f, Health - HealthDepletionFromFood * DeltaTime);
	}

	// Health regenerates when all needs are above critical
	if (!HasAnyCriticalNeed() && Health < 100.0f)
	{
		Health = FMath::Min(100.0f, Health + HealthRegenRate * DeltaTime);
	}

	// Death check
	if (Health <= 0.0f && bIsAlive)
	{
		bIsAlive = false;
		BP_CrewDied();
	}
}

float UCrewNeedsComponent::GetNeedValue(ECrewNeedType NeedType) const
{
	switch (NeedType)
	{
	case ECrewNeedType::Oxygen: return Oxygen;
	case ECrewNeedType::Food:   return Food;
	case ECrewNeedType::Sleep:  return Sleep;
	case ECrewNeedType::Health: return Health;
	default: return 0.0f;
	}
}

bool UCrewNeedsComponent::IsNeedCritical(ECrewNeedType NeedType) const
{
	return GetNeedValue(NeedType) < CriticalThreshold;
}

bool UCrewNeedsComponent::IsNeedLow(ECrewNeedType NeedType) const
{
	return GetNeedValue(NeedType) < LowThreshold;
}

ECrewNeedType UCrewNeedsComponent::GetMostUrgentNeed() const
{
	// Priority order: Oxygen > Food > Sleep > Health
	// Return the first need that's below LowThreshold
	// If none are low, return the lowest one

	if (IsNeedLow(ECrewNeedType::Oxygen)) return ECrewNeedType::Oxygen;
	if (IsNeedLow(ECrewNeedType::Food))   return ECrewNeedType::Food;
	if (IsNeedLow(ECrewNeedType::Sleep))  return ECrewNeedType::Sleep;

	// Nothing is urgent - return the lowest value
	float MinValue = Oxygen;
	ECrewNeedType MinType = ECrewNeedType::Oxygen;

	if (Food < MinValue)  { MinValue = Food;  MinType = ECrewNeedType::Food; }
	if (Sleep < MinValue) { MinValue = Sleep; MinType = ECrewNeedType::Sleep; }

	return MinType;
}

bool UCrewNeedsComponent::HasAnyCriticalNeed() const
{
	return IsNeedCritical(ECrewNeedType::Oxygen)
		|| IsNeedCritical(ECrewNeedType::Food)
		|| IsNeedCritical(ECrewNeedType::Sleep);
}

void UCrewNeedsComponent::ReplenishOxygen(float DeltaTime)
{
	Oxygen = FMath::Min(100.0f, Oxygen + OxygenReplenishRate * DeltaTime);
}

void UCrewNeedsComponent::ReplenishFood(float DeltaTime)
{
	Food = FMath::Min(100.0f, Food + FoodReplenishRate * DeltaTime);
}

void UCrewNeedsComponent::ReplenishSleep(float DeltaTime)
{
	Sleep = FMath::Min(100.0f, Sleep + SleepReplenishRate * DeltaTime);
}

void UCrewNeedsComponent::SetInAtmosphere(bool bHasAtmosphere)
{
	bInAtmosphere = bHasAtmosphere;
}
