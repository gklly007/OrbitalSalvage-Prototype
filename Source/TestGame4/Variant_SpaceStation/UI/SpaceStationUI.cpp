// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpaceStationUI.h"
#include "SpaceStationGameMode.h"
#include "StationSystemsComponent.h"
#include "SpaceStationPlayerController.h"
#include "Kismet/GameplayStatics.h"

void USpaceStationUI::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshDisplay();
}

void USpaceStationUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	RefreshTimer += InDeltaTime;
	if (RefreshTimer >= RefreshInterval)
	{
		RefreshTimer = 0.0f;
		RefreshDisplay();
	}
}

void USpaceStationUI::RefreshDisplay()
{
	ASpaceStationGameMode* GM = Cast<ASpaceStationGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM)
		return;

	UStationSystemsComponent* Systems = GM->GetStationSystems();
	if (Systems)
	{
		DisplayPower = Systems->GetNetPower();
		DisplayPowerGeneration = Systems->TotalPowerGeneration;
		DisplayPowerConsumption = Systems->TotalPowerConsumption;
		bDisplayPowerSufficient = Systems->IsPowerSufficient();

		DisplayOxygen = Systems->GetNetOxygen();
		DisplayOxygenGeneration = Systems->TotalOxygenGeneration;
		DisplayOxygenConsumption = Systems->TotalOxygenConsumption;
		bDisplayOxygenSufficient = Systems->IsOxygenSufficient();
	}

	DisplayFood = GM->GetCurrentFood();
	DisplayCredits = GM->GetCurrentCredits();
	DisplayModuleCount = GM->GetAllModules().Num();
	DisplayCrewCount = GM->GetAllCrew().Num();

	// Check build mode state
	ASpaceStationPlayerController* PC = Cast<ASpaceStationPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	bDisplayInBuildMode = PC ? PC->IsInBuildMode() : false;
	bDisplayInDeleteMode = PC ? PC->IsInDeleteMode() : false;
	DisplayGameSpeed = PC ? PC->GetGameSpeed() : 1.0f;
	bDisplayGamePaused = PC ? PC->IsGamePaused() : false;
	DisplaySelectedCrewCount = PC ? PC->GetSelectedCrewCount() : 0;

	// Generate warning text
	bool bHasWarning = false;
	if (Systems && !Systems->IsPowerSufficient())
	{
		DisplayWarningText = FText::FromString("WARNING: Insufficient Power!");
		bHasWarning = true;
	}
	else if (Systems && !Systems->IsOxygenSufficient())
	{
		DisplayWarningText = FText::FromString("WARNING: Insufficient Oxygen!");
		bHasWarning = true;
	}
	else
	{
		DisplayWarningText = FText::GetEmpty();
	}

	// Fire events
	BP_OnResourcesUpdated();

	if (bHasWarning && !bHadWarning)
	{
		BP_OnWarning(DisplayWarningText);
	}
	else if (!bHasWarning && bHadWarning)
	{
		BP_OnWarningCleared();
	}
	bHadWarning = bHasWarning;
}
