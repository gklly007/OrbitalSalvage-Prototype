// Copyright Epic Games, Inc. All Rights Reserved.

#include "OrbitalStationActor.h"
#include "OrbitalShipPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "UObject/ConstructorHelpers.h"

AOrbitalStationActor::AOrbitalStationActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	MeshComponent->SetSimulatePhysics(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(CylinderMesh.Object);
		MeshComponent->SetRelativeScale3D(FVector(5.0f, 5.0f, 1.5f));
	}

	DockRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DockRangeSphere"));
	DockRangeSphere->SetupAttachment(RootComponent);
	DockRangeSphere->SetSphereRadius(DockRange);
	DockRangeSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool AOrbitalStationActor::CanDockShip(const AOrbitalShipPawn* Ship) const
{
	if (!Ship)
	{
		return false;
	}

	return FVector::DistSquared2D(Ship->GetActorLocation(), GetActorLocation()) <= FMath::Square(DockRange);
}

