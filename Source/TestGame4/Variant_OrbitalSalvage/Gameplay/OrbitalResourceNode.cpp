// Copyright Epic Games, Inc. All Rights Reserved.

#include "OrbitalResourceNode.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AOrbitalResourceNode::AOrbitalResourceNode()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	MeshComponent->SetSimulatePhysics(false);
	MeshComponent->SetGenerateOverlapEvents(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(SphereMesh.Object);
	}
}

float AOrbitalResourceNode::ExtractResource(float RequestedUnits, EOrbitalResourceType& OutResourceType, float& OutUnitVolume, int32& OutCreditValue, bool& bOutRecoveredBlackBox)
{
	bOutRecoveredBlackBox = false;

	if (RequestedUnits <= 0.0f || ResourceUnitsRemaining <= 0.0f)
	{
		OutResourceType = ResourceType;
		OutUnitVolume = UnitVolume;
		OutCreditValue = UnitCreditValue;
		return 0.0f;
	}

	const float EffectiveRequest = RequestedUnits / FMath::Max(MiningResistance, 0.25f);
	const float Extracted = FMath::Min(EffectiveRequest, ResourceUnitsRemaining);

	ResourceUnitsRemaining -= Extracted;
	OutResourceType = ResourceType;
	OutUnitVolume = UnitVolume;
	OutCreditValue = UnitCreditValue;

	if (bContainsBlackBox && NodeKind == EOrbitalResourceNodeKind::Wreck)
	{
		bOutRecoveredBlackBox = true;
		bContainsBlackBox = false;
	}

	if (ResourceUnitsRemaining <= KINDA_SMALL_NUMBER)
	{
		Destroy();
	}

	return Extracted;
}

