// Copyright Epic Games, Inc. All Rights Reserved.

#include "OrbitalShipPawn.h"
#include "ShipSystemsComponent.h"
#include "SalvageComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AOrbitalShipPawn::AOrbitalShipPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	ShipMesh->SetupAttachment(SceneRoot);
	ShipMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ShipMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	ShipMesh->SetSimulatePhysics(true);
	ShipMesh->SetEnableGravity(false);
	ShipMesh->SetLinearDamping(0.2f);
	ShipMesh->SetAngularDamping(1.0f);
	ShipMesh->BodyInstance.bLockXRotation = true;
	ShipMesh->BodyInstance.bLockYRotation = true;
	ShipMesh->BodyInstance.bLockZTranslation = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (ConeMesh.Succeeded())
	{
		ShipMesh->SetStaticMesh(ConeMesh.Object);
		ShipMesh->SetRelativeScale3D(FVector(1.6f, 2.0f, 1.2f));
		ShipMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(ShipMesh);
	SpringArm->TargetArmLength = 2600.0f;
	SpringArm->SetRelativeRotation(FRotator(-63.0f, 0.0f, 0.0f));
	SpringArm->bDoCollisionTest = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 7.0f;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->ProjectionMode = ECameraProjectionMode::Perspective;
	Camera->SetFieldOfView(82.0f);

	ShipSystems = CreateDefaultSubobject<UShipSystemsComponent>(TEXT("ShipSystems"));
	SalvageComponent = CreateDefaultSubobject<USalvageComponent>(TEXT("SalvageComponent"));

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AOrbitalShipPawn::BeginPlay()
{
	Super::BeginPlay();
	SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, 240.0f));
}

void AOrbitalShipPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TickPhysics(DeltaSeconds);
}

void AOrbitalShipPawn::SetMoveInput(const FVector2D& InMoveInput)
{
	MoveInput = InMoveInput.GetClampedToMaxSize(1.0f);
}

void AOrbitalShipPawn::SetTurnInput(float InTurnInput)
{
	TurnInput = FMath::Clamp(InTurnInput, -1.0f, 1.0f);
}

void AOrbitalShipPawn::SetBrakeInput(bool bInBrake)
{
	bBrakeInput = bInBrake;
}

void AOrbitalShipPawn::SetMiningActive(bool bActive)
{
	if (SalvageComponent)
	{
		SalvageComponent->SetMiningActive(bActive);
	}
}

void AOrbitalShipPawn::ToggleFlightAssist()
{
	bFlightAssistEnabled = !bFlightAssistEnabled;
}

float AOrbitalShipPawn::GetCurrentSpeed() const
{
	return ShipMesh ? ShipMesh->GetPhysicsLinearVelocity().Size2D() : 0.0f;
}

void AOrbitalShipPawn::TickPhysics(float DeltaSeconds)
{
	if (!ShipMesh || !ShipSystems || !ShipSystems->IsAlive())
	{
		return;
	}

	const float InputMagnitude = FMath::Clamp(MoveInput.Size() + FMath::Abs(TurnInput), 0.0f, 1.5f);
	if (InputMagnitude > KINDA_SMALL_NUMBER)
	{
		const float FuelCost = ThrustFuelCostPerSecond * InputMagnitude * DeltaSeconds;
		const float PowerCost = ThrusterPowerCostPerSecond * InputMagnitude * DeltaSeconds;

		const bool bHasFuel = ShipSystems->ConsumeFuel(FuelCost);
		const bool bHasPower = ShipSystems->ConsumePower(PowerCost);

		if (bHasFuel && bHasPower)
		{
			const FVector ForwardForce = GetActorForwardVector() * MoveInput.Y * ForwardThrustForce;
			const FVector StrafeForce = GetActorRightVector() * MoveInput.X * StrafeThrustForce;
			ShipMesh->AddForce(ForwardForce + StrafeForce);

			const FVector TorqueVector = FVector(0.0f, 0.0f, TurnInput * TurnTorque);
			ShipMesh->AddTorqueInRadians(TorqueVector);
		}
	}

	if (bBrakeInput)
	{
		const FVector Velocity = ShipMesh->GetPhysicsLinearVelocity();
		const FVector BrakeVector = -Velocity.GetSafeNormal2D() * BrakeForce;
		ShipMesh->AddForce(BrakeVector);
		ShipSystems->AddHeat(4.0f * DeltaSeconds);
	}

	if (bFlightAssistEnabled)
	{
		const FVector Vel = ShipMesh->GetPhysicsLinearVelocity();
		ShipMesh->AddForce(-Vel * AssistLinearDamping * ShipMesh->GetMass());

		const FVector AngularVel = ShipMesh->GetPhysicsAngularVelocityInRadians();
		ShipMesh->AddTorqueInRadians(-AngularVel * AssistAngularDamping * ShipMesh->GetMass());
	}

	FVector ClampedVelocity = ShipMesh->GetPhysicsLinearVelocity();
	const float Speed = ClampedVelocity.Size2D();
	if (Speed > MaxSpeed)
	{
		const FVector FlatVel = FVector(ClampedVelocity.X, ClampedVelocity.Y, 0.0f).GetSafeNormal() * MaxSpeed;
		ClampedVelocity.X = FlatVel.X;
		ClampedVelocity.Y = FlatVel.Y;
		ShipMesh->SetPhysicsLinearVelocity(ClampedVelocity);
	}

	// Keep ship on 2D flight plane.
	FVector Location = GetActorLocation();
	Location.Z = 240.0f;
	SetActorLocation(Location);
}

