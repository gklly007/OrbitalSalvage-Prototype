// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbitalEnemyDrone.generated.h"

class UStaticMeshComponent;
class AOrbitalShipPawn;

/**
 * Lightweight hostile drone for prototype combat pressure.
 */
UCLASS()
class AOrbitalEnemyDrone : public AActor
{
	GENERATED_BODY()

public:
	AOrbitalEnemyDrone();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drone")
	float MaxHealth = 90.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Drone")
	float Health = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drone")
	float MoveSpeed = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drone")
	float ChaseRange = 5500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drone")
	float AttackRange = 550.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drone")
	float HullDamagePerSecond = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drone")
	int32 BountyCredits = 120;

	UFUNCTION(BlueprintCallable, Category="Drone")
	void ApplyMiningDamage(float DamageAmount);

private:
	TObjectPtr<AOrbitalShipPawn> TargetShip;
	float AttackTickAccumulator = 0.0f;

	void AcquireTargetShip();
};

