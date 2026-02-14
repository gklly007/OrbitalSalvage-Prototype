// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShipSystemsComponent.h"

UShipSystemsComponent::UShipSystemsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UShipSystemsComponent::BeginPlay()
{
	Super::BeginPlay();

	Hull = MaxHull;
	Fuel = MaxFuel;
	CurrentPower = MaxPower;
	Heat = 0.0f;
	CargoUsed = 0.0f;

	UpgradeLevels.Add(EOrbitalUpgradeType::Reactor, 0);
	UpgradeLevels.Add(EOrbitalUpgradeType::CargoPod, 0);
	UpgradeLevels.Add(EOrbitalUpgradeType::MiningLaser, 0);
}

void UShipSystemsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentPower = FMath::Clamp(CurrentPower + PassivePowerRegenPerSecond * DeltaTime, 0.0f, MaxPower);
	CurrentPower = FMath::Clamp(CurrentPower - PassivePowerDrainPerSecond * DeltaTime, 0.0f, MaxPower);
	Heat = FMath::Clamp(Heat - HeatDissipationPerSecond * DeltaTime, 0.0f, MaxHeat * 2.0f);

	if (Heat > MaxHeat)
	{
		ApplyHullDamage(OverheatHullDamagePerSecond * DeltaTime);
	}
}

void UShipSystemsComponent::AddHeat(float Amount)
{
	Heat = FMath::Clamp(Heat + Amount, 0.0f, MaxHeat * 2.0f);
}

bool UShipSystemsComponent::ConsumePower(float Amount)
{
	if (CurrentPower < Amount)
	{
		return false;
	}

	CurrentPower -= Amount;
	return true;
}

bool UShipSystemsComponent::ConsumeFuel(float Amount)
{
	if (Fuel < Amount)
	{
		return false;
	}

	Fuel -= Amount;
	return true;
}

void UShipSystemsComponent::ApplyHullDamage(float Amount)
{
	Hull = FMath::Max(0.0f, Hull - Amount);
}

void UShipSystemsComponent::RepairHull(float Amount)
{
	Hull = FMath::Clamp(Hull + Amount, 0.0f, MaxHull);
}

void UShipSystemsComponent::Refuel(float Amount)
{
	Fuel = FMath::Clamp(Fuel + Amount, 0.0f, MaxFuel);
}

float UShipSystemsComponent::AddCargo(EOrbitalResourceType ResourceType, float Units, float UnitVolume, int32 UnitCreditValue)
{
	if (Units <= 0.0f || UnitVolume <= 0.0f)
	{
		return 0.0f;
	}

	const float FreeVolume = FMath::Max(0.0f, CargoCapacity - CargoUsed);
	const float MaxUnitsByVolume = FreeVolume / UnitVolume;
	const float AddedUnits = FMath::Clamp(Units, 0.0f, MaxUnitsByVolume);

	if (AddedUnits <= 0.0f)
	{
		return 0.0f;
	}

	CargoUsed += AddedUnits * UnitVolume;
	CargoUsed = FMath::Min(CargoUsed, CargoCapacity);

	float& CurrentUnits = CargoUnits.FindOrAdd(ResourceType);
	CurrentUnits += AddedUnits;

	CargoUnitPrices.Add(ResourceType, UnitCreditValue);
	return AddedUnits;
}

float UShipSystemsComponent::RemoveCargo(EOrbitalResourceType ResourceType, float UnitsToRemove)
{
	float* FoundUnits = CargoUnits.Find(ResourceType);
	if (!FoundUnits || UnitsToRemove <= 0.0f)
	{
		return 0.0f;
	}

	const float RemovedUnits = FMath::Min(*FoundUnits, UnitsToRemove);
	*FoundUnits -= RemovedUnits;
	if (*FoundUnits <= KINDA_SMALL_NUMBER)
	{
		CargoUnits.Remove(ResourceType);
	}

	const float VolumePerUnit = GetCargoVolumeForType(ResourceType);
	CargoUsed = FMath::Max(0.0f, CargoUsed - RemovedUnits * VolumePerUnit);
	return RemovedUnits;
}

float UShipSystemsComponent::GetCargoUnits(EOrbitalResourceType ResourceType) const
{
	const float* FoundUnits = CargoUnits.Find(ResourceType);
	return FoundUnits ? *FoundUnits : 0.0f;
}

int32 UShipSystemsComponent::SellAllCargo()
{
	int32 TotalRevenue = 0;

	for (const TPair<EOrbitalResourceType, float>& Entry : CargoUnits)
	{
		const int32 UnitPrice = CargoUnitPrices.FindRef(Entry.Key);
		const int32 UnitsRounded = FMath::FloorToInt(Entry.Value);
		TotalRevenue += UnitsRounded * UnitPrice;
	}

	Credits += TotalRevenue;
	CargoUnits.Empty();
	CargoUsed = 0.0f;

	return TotalRevenue;
}

int32 UShipSystemsComponent::GetUpgradeCost(EOrbitalUpgradeType UpgradeType) const
{
	const int32 Level = UpgradeLevels.FindRef(UpgradeType);
	const int32 BaseCost = [&]()
	{
		switch (UpgradeType)
		{
		case EOrbitalUpgradeType::Reactor:
			return 1300;
		case EOrbitalUpgradeType::CargoPod:
			return 1000;
		case EOrbitalUpgradeType::MiningLaser:
			return 1200;
		default:
			return 1200;
		}
	}();

	return BaseCost + (Level * 800);
}

bool UShipSystemsComponent::PurchaseUpgrade(EOrbitalUpgradeType UpgradeType)
{
	const int32 Cost = GetUpgradeCost(UpgradeType);
	if (Credits < Cost)
	{
		return false;
	}

	int32& Level = UpgradeLevels.FindOrAdd(UpgradeType);
	if (Level >= 3)
	{
		return false;
	}

	Credits -= Cost;
	++Level;

	switch (UpgradeType)
	{
	case EOrbitalUpgradeType::Reactor:
		MaxPower += 20.0f;
		PassivePowerRegenPerSecond += 4.0f;
		CurrentPower = MaxPower;
		break;
	case EOrbitalUpgradeType::CargoPod:
		CargoCapacity += 70.0f;
		break;
	case EOrbitalUpgradeType::MiningLaser:
		MaxHeat += 12.0f;
		break;
	default:
		break;
	}

	return true;
}

float UShipSystemsComponent::GetMiningYieldMultiplier() const
{
	const int32 LaserLevel = UpgradeLevels.FindRef(EOrbitalUpgradeType::MiningLaser);
	return 1.0f + (LaserLevel * 0.35f);
}

float UShipSystemsComponent::GetMiningHeatMultiplier() const
{
	const int32 ReactorLevel = UpgradeLevels.FindRef(EOrbitalUpgradeType::Reactor);
	return FMath::Max(0.45f, 1.0f - (ReactorLevel * 0.12f));
}

float UShipSystemsComponent::GetCargoVolumeForType(EOrbitalResourceType ResourceType) const
{
	switch (ResourceType)
	{
	case EOrbitalResourceType::Ore:
		return 1.5f;
	case EOrbitalResourceType::Salvage:
		return 1.0f;
	case EOrbitalResourceType::FuelCell:
		return 2.0f;
	case EOrbitalResourceType::BlackBox:
		return 8.0f;
	default:
		return 1.0f;
	}
}
