// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "OrbitalShipPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UStaticMeshComponent;
class USceneComponent;
class UShipSystemsComponent;
class USalvageComponent;

/**
 * Physics-based top-down ship pawn for Orbital Salvage gameplay.
 */
UCLASS()
class AOrbitalShipPawn : public APawn
{
	GENERATED_BODY()

public:
	AOrbitalShipPawn();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* ShipMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UShipSystemsComponent* ShipSystems;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USalvageComponent* SalvageComponent;

	// Movement tuning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight")
	float ForwardThrustForce = 360000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight")
	float StrafeThrustForce = 220000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight")
	float TurnTorque = 7000000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight")
	float BrakeForce = 260000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight")
	float MaxSpeed = 4300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight")
	float ThrustFuelCostPerSecond = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight")
	float ThrusterPowerCostPerSecond = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight")
	bool bFlightAssistEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight")
	float AssistLinearDamping = 0.9f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight")
	float AssistAngularDamping = 4.5f;

public:
	UFUNCTION(BlueprintCallable, Category="Flight")
	void SetMoveInput(const FVector2D& InMoveInput);

	UFUNCTION(BlueprintCallable, Category="Flight")
	void SetTurnInput(float InTurnInput);

	UFUNCTION(BlueprintCallable, Category="Flight")
	void SetBrakeInput(bool bInBrake);

	UFUNCTION(BlueprintCallable, Category="Flight")
	void SetMiningActive(bool bActive);

	UFUNCTION(BlueprintCallable, Category="Flight")
	void ToggleFlightAssist();

	UFUNCTION(BlueprintPure, Category="Flight")
	FVector2D GetMoveInput() const { return MoveInput; }

	UFUNCTION(BlueprintPure, Category="Flight")
	float GetTurnInput() const { return TurnInput; }

	UFUNCTION(BlueprintPure, Category="Ship")
	UShipSystemsComponent* GetShipSystems() const { return ShipSystems; }

	UFUNCTION(BlueprintPure, Category="Ship")
	USalvageComponent* GetSalvageComponent() const { return SalvageComponent; }

	UFUNCTION(BlueprintPure, Category="Flight")
	float GetCurrentSpeed() const;

private:
	FVector2D MoveInput = FVector2D::ZeroVector;
	float TurnInput = 0.0f;
	bool bBrakeInput = false;

	void TickPhysics(float DeltaSeconds);
};
