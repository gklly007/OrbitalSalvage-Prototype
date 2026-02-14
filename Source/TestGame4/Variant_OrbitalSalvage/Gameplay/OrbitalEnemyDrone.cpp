// Copyright Epic Games, Inc. All Rights Reserved.

#include "OrbitalEnemyDrone.h"
#include "OrbitalShipPawn.h"
#include "ShipSystemsComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AOrbitalEnemyDrone::AOrbitalEnemyDrone()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	MeshComponent->SetSimulatePhysics(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(SphereMesh.Object);
		MeshComponent->SetRelativeScale3D(FVector(1.8f, 1.8f, 1.8f));
	}
}

void AOrbitalEnemyDrone::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	AcquireTargetShip();
}

void AOrbitalEnemyDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(TargetShip))
	{
		AcquireTargetShip();
		if (!TargetShip)
		{
			return;
		}
	}

	const FVector ShipLocation = TargetShip->GetActorLocation();
	const FVector MyLocation = GetActorLocation();
	const FVector Delta = ShipLocation - MyLocation;
	const float Dist2D = Delta.Size2D();

	if (Dist2D > ChaseRange)
	{
		return;
	}

	const FVector Dir2D = FVector(Delta.X, Delta.Y, 0.0f).GetSafeNormal();
	const FVector NewLocation = MyLocation + Dir2D * MoveSpeed * DeltaTime;
	SetActorLocation(FVector(NewLocation.X, NewLocation.Y, MyLocation.Z));

	const FRotator Facing = Dir2D.Rotation();
	SetActorRotation(FRotator(0.0f, Facing.Yaw, 0.0f));

	if (Dist2D <= AttackRange)
	{
		AttackTickAccumulator += DeltaTime;
		if (AttackTickAccumulator >= 0.2f)
		{
			AttackTickAccumulator = 0.0f;

			if (UShipSystemsComponent* Systems = TargetShip->GetShipSystems())
			{
				Systems->ApplyHullDamage(HullDamagePerSecond * 0.2f);
			}
		}
	}
}

void AOrbitalEnemyDrone::ApplyMiningDamage(float DamageAmount)
{
	Health -= DamageAmount;
	if (Health <= 0.0f)
	{
		if (IsValid(TargetShip))
		{
			if (UShipSystemsComponent* Systems = TargetShip->GetShipSystems())
			{
				Systems->Credits += BountyCredits;
			}
		}

		Destroy();
	}
}

void AOrbitalEnemyDrone::AcquireTargetShip()
{
	if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		TargetShip = Cast<AOrbitalShipPawn>(Pawn);
	}
}

