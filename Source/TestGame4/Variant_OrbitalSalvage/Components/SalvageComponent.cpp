// Copyright Epic Games, Inc. All Rights Reserved.

#include "SalvageComponent.h"
#include "ShipSystemsComponent.h"
#include "OrbitalGameMode.h"
#include "OrbitalResourceNode.h"
#include "OrbitalEnemyDrone.h"
#include "OrbitalTypes.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

USalvageComponent::USalvageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USalvageComponent::BeginPlay()
{
	Super::BeginPlay();

	ShipSystems = GetOwner()->FindComponentByClass<UShipSystemsComponent>();
}

void USalvageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bMiningActive)
	{
		CurrentTarget = nullptr;
		return;
	}

	TickMining(DeltaTime);
}

void USalvageComponent::SetMiningActive(bool bActive)
{
	bMiningActive = bActive;

	if (!bMiningActive)
	{
		CurrentTarget = nullptr;
	}
}

void USalvageComponent::TickMining(float DeltaTime)
{
	if (!ShipSystems)
	{
		return;
	}

	const float PowerCost = BasePowerCostPerSecond * DeltaTime;
	const float FuelCost = BaseFuelCostPerSecond * DeltaTime;

	if (!ShipSystems->ConsumePower(PowerCost) || !ShipSystems->ConsumeFuel(FuelCost))
	{
		bMiningActive = false;
		CurrentTarget = nullptr;
		return;
	}

	ShipSystems->AddHeat(BaseHeatPerSecond * ShipSystems->GetMiningHeatMultiplier() * DeltaTime);

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector Start = GetOwner()->GetActorLocation();
	const FVector End = Start + GetOwner()->GetActorForwardVector() * MiningRange;

	FHitResult Hit;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SalvageTrace), false, GetOwner());
	QueryParams.bTraceComplex = false;

	World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams);
	DrawDebugLine(World, Start, End, FColor::Cyan, false, 0.02f, 0, 1.0f);

	if (!Hit.bBlockingHit || !Hit.GetActor())
	{
		CurrentTarget = nullptr;
		return;
	}

	CurrentTarget = Hit.GetActor();

	if (AOrbitalResourceNode* ResourceNode = Cast<AOrbitalResourceNode>(Hit.GetActor()))
	{
		EOrbitalResourceType ResourceType = EOrbitalResourceType::Ore;
		float UnitVolume = 1.0f;
		int32 UnitCreditValue = 1;
		bool bRecoveredBlackBox = false;

		const float Requested = BaseMiningRatePerSecond * ShipSystems->GetMiningYieldMultiplier() * DeltaTime;
		const float ExtractedUnits = ResourceNode->ExtractResource(Requested, ResourceType, UnitVolume, UnitCreditValue, bRecoveredBlackBox);
		if (ExtractedUnits > 0.0f)
		{
			ShipSystems->AddCargo(ResourceType, ExtractedUnits, UnitVolume, UnitCreditValue);
		}

		if (bRecoveredBlackBox)
		{
			ShipSystems->AddCargo(EOrbitalResourceType::BlackBox, 1.0f, 8.0f, 0);

			if (AOrbitalGameMode* GM = World->GetAuthGameMode<AOrbitalGameMode>())
			{
				GM->HandleBlackBoxRecovered();
			}
		}
		return;
	}

	if (AOrbitalEnemyDrone* EnemyDrone = Cast<AOrbitalEnemyDrone>(Hit.GetActor()))
	{
		EnemyDrone->ApplyMiningDamage(MiningDamagePerSecond * DeltaTime);
	}
}

