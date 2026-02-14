// Copyright Epic Games, Inc. All Rights Reserved.

#include "MissionComponent.h"

UMissionComponent::UMissionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMissionComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentStage = EOrbitalMissionStage::RetrieveBlackBox;
	bHasRecoveredBlackBox = false;
}

FText UMissionComponent::GetObjectiveText() const
{
	switch (CurrentStage)
	{
	case EOrbitalMissionStage::RetrieveBlackBox:
		return FText::FromString(TEXT("Objective: Recover the black box from a wreck in the Belt."));
	case EOrbitalMissionStage::JumpToRuins:
		return FText::FromString(TEXT("Objective: Reach a jump gate and travel to the Ruins sector."));
	case EOrbitalMissionStage::ExtractAtBeacon:
		return FText::FromString(TEXT("Objective: Dock at the extraction beacon in the Ruins."));
	case EOrbitalMissionStage::Completed:
		return FText::FromString(TEXT("Mission Complete: Black box delivered."));
	default:
		return FText::FromString(TEXT("Objective unavailable."));
	}
}

void UMissionComponent::MarkBlackBoxRecovered()
{
	bHasRecoveredBlackBox = true;

	if (CurrentStage == EOrbitalMissionStage::RetrieveBlackBox)
	{
		CurrentStage = EOrbitalMissionStage::JumpToRuins;
	}
}

void UMissionComponent::NotifyEnteredSector(EOrbitalSectorId Sector)
{
	if (CurrentStage == EOrbitalMissionStage::JumpToRuins && Sector == EOrbitalSectorId::Ruins)
	{
		CurrentStage = EOrbitalMissionStage::ExtractAtBeacon;
	}
}

bool UMissionComponent::TryCompleteExtraction(int32& OutReward)
{
	OutReward = 0;

	if (CurrentStage != EOrbitalMissionStage::ExtractAtBeacon || !bHasRecoveredBlackBox)
	{
		return false;
	}

	CurrentStage = EOrbitalMissionStage::Completed;
	OutReward = CompletionReward;
	return true;
}

