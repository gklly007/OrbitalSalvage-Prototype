// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpaceStationHUD.h"
#include "CrewMember.h"
#include "StationModule.h"
#include "SpaceStationGameMode.h"
#include "StationSystemsComponent.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Canvas.h"
#include "Kismet/GameplayStatics.h"

ASpaceStationHUD::ASpaceStationHUD()
{
}

void ASpaceStationHUD::BeginPlay()
{
	Super::BeginPlay();

	CreateMainHUD();
}

void ASpaceStationHUD::DrawHUD()
{
	Super::DrawHUD();

	DrawCrewSelection();
	DrawModuleConnections();
}

void ASpaceStationHUD::ToggleBuildMenu()
{
	if (bBuildMenuVisible)
	{
		HideBuildMenu();
	}
	else
	{
		ShowBuildMenu();
	}
}

void ASpaceStationHUD::ShowBuildMenu()
{
	if (!BuildMenuWidget && BuildMenuWidgetClass)
	{
		BuildMenuWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), BuildMenuWidgetClass);
	}

	if (BuildMenuWidget)
	{
		BuildMenuWidget->AddToViewport(10);
		bBuildMenuVisible = true;
	}
}

void ASpaceStationHUD::HideBuildMenu()
{
	if (BuildMenuWidget)
	{
		BuildMenuWidget->RemoveFromParent();
		bBuildMenuVisible = false;
	}
}

void ASpaceStationHUD::CreateMainHUD()
{
	if (MainHUDWidget)
		return;

	if (MainHUDWidgetClass)
	{
		MainHUDWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), MainHUDWidgetClass);
		if (MainHUDWidget)
		{
			MainHUDWidget->AddToViewport(0);
		}
	}
}

void ASpaceStationHUD::DrawCrewSelection()
{
	ASpaceStationGameMode* GM = GetWorld()->GetAuthGameMode<ASpaceStationGameMode>();
	if (!GM)
		return;

	const TArray<ACrewMember*>& AllCrew = GM->GetAllCrew();
	for (ACrewMember* Crew : AllCrew)
	{
		if (Crew && Crew->bIsSelected)
		{
			DrawWorldActorBounds(Crew, FLinearColor::Green, 2.0f);
		}
	}
}

void ASpaceStationHUD::DrawModuleConnections()
{
	ASpaceStationGameMode* GM = GetWorld()->GetAuthGameMode<ASpaceStationGameMode>();
	if (!GM)
		return;

	// Draw lines between connected modules
	TSet<TPair<AStationModule*, AStationModule*>> DrawnConnections;

	for (AStationModule* Module : GM->GetAllModules())
	{
		if (!Module || !Module->bIsPlaced)
			continue;

		for (AStationModule* Connected : Module->ConnectedModules)
		{
			if (!Connected)
				continue;

			// Avoid drawing the same connection twice
			auto Pair = TPair<AStationModule*, AStationModule*>(
				Module < Connected ? Module : Connected,
				Module < Connected ? Connected : Module
			);

			if (DrawnConnections.Contains(Pair))
				continue;
			DrawnConnections.Add(Pair);

			// Draw connection line
			FVector2D Start2D, End2D;
			if (WorldToScreen(Module->GetActorLocation(), Start2D) &&
				WorldToScreen(Connected->GetActorLocation(), End2D))
			{
				FLinearColor ConnectionColor = (Module->bIsPowered && Connected->bIsPowered)
					? FLinearColor(0.0f, 0.8f, 1.0f, 0.5f)  // Cyan for powered
					: FLinearColor(0.5f, 0.5f, 0.5f, 0.3f);  // Grey for unpowered

				DrawLine(Start2D.X, Start2D.Y, End2D.X, End2D.Y,
					ConnectionColor, 1.5f);
			}
		}
	}
}

void ASpaceStationHUD::DrawResourceOverlay()
{
	// This is a fallback if no UMG widget is assigned
	// The UMG widget (MainHUDWidget) is preferred for resource display
}

void ASpaceStationHUD::DrawWorldActorBounds(AActor* Actor, FLinearColor Color, float Thickness)
{
	if (!Actor)
		return;

	// Get actor bounds
	FVector Origin, BoxExtent;
	Actor->GetActorBounds(false, Origin, BoxExtent);

	// Project 8 corners of the bounding box to screen
	TArray<FVector2D> ScreenPoints;
	for (int32 X = -1; X <= 1; X += 2)
	{
		for (int32 Y = -1; Y <= 1; Y += 2)
		{
			for (int32 Z = -1; Z <= 1; Z += 2)
			{
				FVector Corner = Origin + FVector(BoxExtent.X * X, BoxExtent.Y * Y, BoxExtent.Z * Z);
				FVector2D ScreenPoint;
				if (WorldToScreen(Corner, ScreenPoint))
				{
					ScreenPoints.Add(ScreenPoint);
				}
			}
		}
	}

	if (ScreenPoints.Num() < 2)
		return;

	// Find screen-space bounding box
	FVector2D Min = ScreenPoints[0];
	FVector2D Max = ScreenPoints[0];
	for (const FVector2D& Point : ScreenPoints)
	{
		Min.X = FMath::Min(Min.X, Point.X);
		Min.Y = FMath::Min(Min.Y, Point.Y);
		Max.X = FMath::Max(Max.X, Point.X);
		Max.Y = FMath::Max(Max.Y, Point.Y);
	}

	// Add padding
	float Padding = 5.0f;
	Min -= FVector2D(Padding, Padding);
	Max += FVector2D(Padding, Padding);

	// Draw selection rectangle
	DrawLine(Min.X, Min.Y, Max.X, Min.Y, Color, Thickness); // Top
	DrawLine(Max.X, Min.Y, Max.X, Max.Y, Color, Thickness); // Right
	DrawLine(Max.X, Max.Y, Min.X, Max.Y, Color, Thickness); // Bottom
	DrawLine(Min.X, Max.Y, Min.X, Min.Y, Color, Thickness); // Left
}

bool ASpaceStationHUD::WorldToScreen(const FVector& WorldLocation, FVector2D& ScreenLocation) const
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
		return false;

	return PC->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation);
}
