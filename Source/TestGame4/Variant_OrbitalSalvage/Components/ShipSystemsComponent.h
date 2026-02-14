// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OrbitalTypes.h"
#include "ShipSystemsComponent.generated.h"

/**
 * Core ship simulation for the Orbital Salvage mode.
 * Tracks hull, power, heat, fuel, cargo and credits.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UShipSystemsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UShipSystemsComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Hull
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship|Hull")
	float MaxHull = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship|Hull")
	float Hull = 100.0f;

	// Fuel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship|Fuel")
	float MaxFuel = 350.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship|Fuel")
	float Fuel = 350.0f;

	// Power
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship|Power")
	float MaxPower = 120.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship|Power")
	float CurrentPower = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship|Power")
	float PassivePowerRegenPerSecond = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship|Power")
	float PassivePowerDrainPerSecond = 1.0f;

	// Heat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship|Heat")
	float MaxHeat = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship|Heat")
	float Heat = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship|Heat")
	float HeatDissipationPerSecond = 16.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship|Heat")
	float OverheatHullDamagePerSecond = 5.0f;

	// Cargo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship|Cargo")
	float CargoCapacity = 220.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship|Cargo")
	float CargoUsed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship|Cargo")
	TMap<EOrbitalResourceType, float> CargoUnits;

	// Economy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship|Economy")
	int32 Credits = 2500;

	// Upgrade Levels
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ship|Upgrade")
	TMap<EOrbitalUpgradeType, int32> UpgradeLevels;

public:
	UFUNCTION(BlueprintPure, Category="Ship")
	bool IsAlive() const { return Hull > 0.0f; }

	UFUNCTION(BlueprintCallable, Category="Ship")
	void AddHeat(float Amount);

	UFUNCTION(BlueprintCallable, Category="Ship")
	bool ConsumePower(float Amount);

	UFUNCTION(BlueprintCallable, Category="Ship")
	bool ConsumeFuel(float Amount);

	UFUNCTION(BlueprintCallable, Category="Ship")
	void ApplyHullDamage(float Amount);

	UFUNCTION(BlueprintCallable, Category="Ship")
	void RepairHull(float Amount);

	UFUNCTION(BlueprintCallable, Category="Ship")
	void Refuel(float Amount);

	UFUNCTION(BlueprintCallable, Category="Ship|Cargo")
	float AddCargo(EOrbitalResourceType ResourceType, float Units, float UnitVolume, int32 UnitCreditValue);

	UFUNCTION(BlueprintCallable, Category="Ship|Cargo")
	float RemoveCargo(EOrbitalResourceType ResourceType, float UnitsToRemove);

	UFUNCTION(BlueprintPure, Category="Ship|Cargo")
	float GetCargoUnits(EOrbitalResourceType ResourceType) const;

	UFUNCTION(BlueprintCallable, Category="Ship|Cargo")
	int32 SellAllCargo();

	UFUNCTION(BlueprintPure, Category="Ship|Economy")
	int32 GetUpgradeCost(EOrbitalUpgradeType UpgradeType) const;

	UFUNCTION(BlueprintCallable, Category="Ship|Economy")
	bool PurchaseUpgrade(EOrbitalUpgradeType UpgradeType);

	UFUNCTION(BlueprintPure, Category="Ship|Upgrade")
	float GetMiningYieldMultiplier() const;

	UFUNCTION(BlueprintPure, Category="Ship|Upgrade")
	float GetMiningHeatMultiplier() const;

private:
	TMap<EOrbitalResourceType, int32> CargoUnitPrices;

	float GetCargoVolumeForType(EOrbitalResourceType ResourceType) const;
};

