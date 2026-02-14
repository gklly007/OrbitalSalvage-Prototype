// Copyright Epic Games, Inc. All Rights Reserved.

#include "OrbitalJumpGateActor.h"
#include "OrbitalShipPawn.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AOrbitalJumpGateActor::AOrbitalJumpGateActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	MeshComponent->SetSimulatePhysics(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> TorusMesh(TEXT("/Engine/BasicShapes/Torus.Torus"));
	if (TorusMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(TorusMesh.Object);
		MeshComponent->SetRelativeScale3D(FVector(6.0f, 6.0f, 6.0f));
	}
	else
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
		if (SphereMesh.Succeeded())
		{
			MeshComponent->SetStaticMesh(SphereMesh.Object);
			MeshComponent->SetRelativeScale3D(FVector(5.5f, 5.5f, 5.5f));
		}
	}
}

bool AOrbitalJumpGateActor::CanShipUseGate(const AOrbitalShipPawn* Ship) const
{
	if (!Ship)
	{
		return false;
	}

	return FVector::DistSquared2D(Ship->GetActorLocation(), GetActorLocation()) <= FMath::Square(ActivationRange);
}

