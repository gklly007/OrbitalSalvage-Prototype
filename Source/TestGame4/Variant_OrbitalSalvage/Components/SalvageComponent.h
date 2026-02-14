// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SalvageComponent.generated.h"

class UShipSystemsComponent;
class AOrbitalResourceNode;
class AOrbitalEnemyDrone;

/**
 * Handles mining/salvage laser interactions for the player's ship.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class USalvageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USalvageComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Salvage")
	float MiningRange = 3500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Salvage")
	float BaseMiningRatePerSecond = 28.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Salvage")
	float BasePowerCostPerSecond = 16.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Salvage")
	float BaseHeatPerSecond = 13.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Salvage")
	float BaseFuelCostPerSecond = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Salvage")
	float MiningDamagePerSecond = 20.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Salvage")
	bool bMiningActive = false;

	UFUNCTION(BlueprintCallable, Category="Salvage")
	void SetMiningActive(bool bActive);

	UFUNCTION(BlueprintPure, Category="Salvage")
	AActor* GetCurrentTarget() const { return CurrentTarget; }

private:
	UPROPERTY()
	UShipSystemsComponent* ShipSystems = nullptr;

	UPROPERTY()
	AActor* CurrentTarget = nullptr;

	void TickMining(float DeltaTime);
};

