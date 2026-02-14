// Copyright Epic Games, Inc. All Rights Reserved.

#include "OrbitalHUD.h"
#include "OrbitalGameMode.h"
#include "OrbitalShipPawn.h"
#include "ShipSystemsComponent.h"
#include "SalvageComponent.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void AOrbitalHUD::DrawHUD()
{
	Super::DrawHUD();

	AOrbitalGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<AOrbitalGameMode>() : nullptr;
	AOrbitalShipPawn* Ship = Cast<AOrbitalShipPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (!GM || !Ship || !Ship->GetShipSystems() || !Canvas)
	{
		return;
	}

	UShipSystemsComponent* Systems = Ship->GetShipSystems();
	USalvageComponent* Salvage = Ship->GetSalvageComponent();

	float Y = 20.0f;
	const float X = 20.0f;

	const FString Header = FString::Printf(TEXT("Orbital Salvage | Sector: %s"), *GM->GetCurrentSectorName());
	DrawText(Header, FColor::Cyan, X, Y, GEngine->GetLargeFont());
	Y += 30.0f;

	DrawText(FString::Printf(TEXT("Hull: %.0f / %.0f"), Systems->Hull, Systems->MaxHull), FColor::White, X, Y);
	Y += 18.0f;
	DrawText(FString::Printf(TEXT("Fuel: %.0f / %.0f"), Systems->Fuel, Systems->MaxFuel), FColor::White, X, Y);
	Y += 18.0f;
	DrawText(FString::Printf(TEXT("Power: %.0f / %.0f"), Systems->CurrentPower, Systems->MaxPower), FColor::White, X, Y);
	Y += 18.0f;
	DrawText(FString::Printf(TEXT("Heat: %.0f / %.0f"), Systems->Heat, Systems->MaxHeat), FColor::White, X, Y);
	Y += 18.0f;
	DrawText(FString::Printf(TEXT("Cargo: %.0f / %.0f"), Systems->CargoUsed, Systems->CargoCapacity), FColor::White, X, Y);
	Y += 18.0f;
	DrawText(FString::Printf(TEXT("Credits: %d"), Systems->Credits), FColor::Yellow, X, Y);
	Y += 22.0f;

	const float Speed = Ship->GetCurrentSpeed();
	DrawText(FString::Printf(TEXT("Speed: %.0f m/s"), Speed), FColor::White, X, Y);
	Y += 18.0f;

	const FString MiningState = Salvage && Salvage->bMiningActive ? TEXT("MINING: ON") : TEXT("MINING: OFF");
	DrawText(MiningState, Salvage && Salvage->bMiningActive ? FColor::Green : FColor::Silver, X, Y);
	Y += 24.0f;

	DrawText(GM->GetObjectiveText().ToString(), FColor::Green, X, Y);
	Y += 22.0f;

	if (!GM->LastStatusMessage.IsEmpty())
	{
		DrawText(FString::Printf(TEXT("Status: %s"), *GM->LastStatusMessage), FColor::Orange, X, Y);
	}

	const float BottomY = Canvas->ClipY - 120.0f;
	DrawText(TEXT("Controls: WASD move, Q/E turn, LMB mine, SPACE brake, F interact"), FColor::Silver, 20.0f, BottomY);
	DrawText(TEXT("Upgrades: 1 Reactor, 2 Cargo, 3 Mining | T flight assist | MouseWheel zoom"), FColor::Silver, 20.0f, BottomY + 18.0f);
}

