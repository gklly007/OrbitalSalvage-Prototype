// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpaceStationPlayerController.h"
#include "SpaceStationPawn.h"
#include "SpaceStationGameMode.h"
#include "SpaceStationHUD.h"
#include "StationModule.h"
#include "StationGrid.h"
#include "CrewMember.h"
#include "CrewAIController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputModifiers.h"
#include "Engine/World.h"

ASpaceStationPlayerController::ASpaceStationPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	bShowMouseCursor = true;
}

void ASpaceStationPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Create Input Actions programmatically if not assigned via Blueprint
	if (!CameraPanAction)
	{
		CameraPanAction = NewObject<UInputAction>(this, TEXT("IA_CameraPan"));
		CameraPanAction->ValueType = EInputActionValueType::Axis2D;
	}
	if (!CameraZoomAction)
	{
		CameraZoomAction = NewObject<UInputAction>(this, TEXT("IA_CameraZoom"));
		CameraZoomAction->ValueType = EInputActionValueType::Axis1D;
	}
	if (!ResetCameraAction)
	{
		ResetCameraAction = NewObject<UInputAction>(this, TEXT("IA_ResetCamera"));
		ResetCameraAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!SelectAction)
	{
		SelectAction = NewObject<UInputAction>(this, TEXT("IA_Select"));
		SelectAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!PlaceModuleAction)
	{
		PlaceModuleAction = NewObject<UInputAction>(this, TEXT("IA_PlaceModule"));
		PlaceModuleAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!CancelPlacementAction)
	{
		CancelPlacementAction = NewObject<UInputAction>(this, TEXT("IA_CancelPlacement"));
		CancelPlacementAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!RotatePreviewAction)
	{
		RotatePreviewAction = NewObject<UInputAction>(this, TEXT("IA_RotatePreview"));
		RotatePreviewAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!CommandCrewAction)
	{
		CommandCrewAction = NewObject<UInputAction>(this, TEXT("IA_CommandCrew"));
		CommandCrewAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!OpenBuildMenuAction)
	{
		OpenBuildMenuAction = NewObject<UInputAction>(this, TEXT("IA_OpenBuildMenu"));
		OpenBuildMenuAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!DeleteModuleAction)
	{
		DeleteModuleAction = NewObject<UInputAction>(this, TEXT("IA_DeleteModule"));
		DeleteModuleAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!SpawnCrewAction)
	{
		SpawnCrewAction = NewObject<UInputAction>(this, TEXT("IA_SpawnCrew"));
		SpawnCrewAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!TogglePauseAction)
	{
		TogglePauseAction = NewObject<UInputAction>(this, TEXT("IA_TogglePause"));
		TogglePauseAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!SpeedUpAction)
	{
		SpeedUpAction = NewObject<UInputAction>(this, TEXT("IA_SpeedUp"));
		SpeedUpAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!SpeedDownAction)
	{
		SpeedDownAction = NewObject<UInputAction>(this, TEXT("IA_SpeedDown"));
		SpeedDownAction->ValueType = EInputActionValueType::Boolean;
	}

	// Create Input Mapping Context programmatically if not assigned
	if (!SpaceStationMappingContext)
	{
		SpaceStationMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_SpaceStation_Runtime"));
	}

	// Always add programmatic key bindings to ensure they work
	{
		// Clear existing mappings and rebuild
		SpaceStationMappingContext->UnmapAll();

		// WASD Camera Pan
		// W = Forward (+Y)
		FEnhancedActionKeyMapping& MapW = SpaceStationMappingContext->MapKey(CameraPanAction, EKeys::W);
		UInputModifierSwizzleAxis* SwizzleW = NewObject<UInputModifierSwizzleAxis>(SpaceStationMappingContext);
		SwizzleW->Order = EInputAxisSwizzle::YXZ;
		MapW.Modifiers.Add(SwizzleW);

		// S = Backward (-Y)
		FEnhancedActionKeyMapping& MapS = SpaceStationMappingContext->MapKey(CameraPanAction, EKeys::S);
		UInputModifierSwizzleAxis* SwizzleS = NewObject<UInputModifierSwizzleAxis>(SpaceStationMappingContext);
		SwizzleS->Order = EInputAxisSwizzle::YXZ;
		UInputModifierNegate* NegateS = NewObject<UInputModifierNegate>(SpaceStationMappingContext);
		MapS.Modifiers.Add(SwizzleS);
		MapS.Modifiers.Add(NegateS);

		// D = Right (+X)
		SpaceStationMappingContext->MapKey(CameraPanAction, EKeys::D);

		// A = Left (-X)
		FEnhancedActionKeyMapping& MapA = SpaceStationMappingContext->MapKey(CameraPanAction, EKeys::A);
		UInputModifierNegate* NegateA = NewObject<UInputModifierNegate>(SpaceStationMappingContext);
		MapA.Modifiers.Add(NegateA);

		// Mouse Wheel Zoom
		SpaceStationMappingContext->MapKey(CameraZoomAction, EKeys::MouseWheelAxis);

		// Home = Reset Camera
		SpaceStationMappingContext->MapKey(ResetCameraAction, EKeys::Home);

		// Left Click = Select + Place Module
		SpaceStationMappingContext->MapKey(SelectAction, EKeys::LeftMouseButton);
		SpaceStationMappingContext->MapKey(PlaceModuleAction, EKeys::LeftMouseButton);

		// Right Click = Cancel + Command Crew
		SpaceStationMappingContext->MapKey(CancelPlacementAction, EKeys::RightMouseButton);
		SpaceStationMappingContext->MapKey(CancelPlacementAction, EKeys::Escape);
		SpaceStationMappingContext->MapKey(CommandCrewAction, EKeys::RightMouseButton);

		// R = Rotate
		SpaceStationMappingContext->MapKey(RotatePreviewAction, EKeys::R);

		// B = Build Menu
		SpaceStationMappingContext->MapKey(OpenBuildMenuAction, EKeys::B);

		// X = Delete Module
		SpaceStationMappingContext->MapKey(DeleteModuleAction, EKeys::X);

		// C = Spawn Crew
		SpaceStationMappingContext->MapKey(SpawnCrewAction, EKeys::C);

		// P = Toggle Pause
		SpaceStationMappingContext->MapKey(TogglePauseAction, EKeys::P);

		// + = Speed Up (numpad and regular)
		SpaceStationMappingContext->MapKey(SpeedUpAction, EKeys::Add);
		SpaceStationMappingContext->MapKey(SpeedUpAction, EKeys::Equals);

		// - = Speed Down (numpad and regular)
		SpaceStationMappingContext->MapKey(SpeedDownAction, EKeys::Subtract);
		SpaceStationMappingContext->MapKey(SpeedDownAction, EKeys::Hyphen);
	}

	// Add mapping context to Enhanced Input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(SpaceStationMappingContext, 0);
	}

	// Bind input actions to handler functions
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EIC->BindAction(CameraPanAction, ETriggerEvent::Triggered, this, &ASpaceStationPlayerController::CameraPan);
		EIC->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &ASpaceStationPlayerController::CameraZoom);
		EIC->BindAction(ResetCameraAction, ETriggerEvent::Started, this, &ASpaceStationPlayerController::OnResetCamera);
		EIC->BindAction(SelectAction, ETriggerEvent::Started, this, &ASpaceStationPlayerController::OnSelect);
		EIC->BindAction(PlaceModuleAction, ETriggerEvent::Started, this, &ASpaceStationPlayerController::OnPlaceModule);
		EIC->BindAction(CancelPlacementAction, ETriggerEvent::Started, this, &ASpaceStationPlayerController::OnCancelPlacement);
		EIC->BindAction(RotatePreviewAction, ETriggerEvent::Started, this, &ASpaceStationPlayerController::OnRotatePreview);
		EIC->BindAction(CommandCrewAction, ETriggerEvent::Started, this, &ASpaceStationPlayerController::OnCommandCrew);
		EIC->BindAction(OpenBuildMenuAction, ETriggerEvent::Started, this, &ASpaceStationPlayerController::OnOpenBuildMenu);
		EIC->BindAction(DeleteModuleAction, ETriggerEvent::Started, this, &ASpaceStationPlayerController::OnDeleteModule);
		EIC->BindAction(SpawnCrewAction, ETriggerEvent::Started, this, &ASpaceStationPlayerController::OnSpawnCrew);
		EIC->BindAction(TogglePauseAction, ETriggerEvent::Started, this, &ASpaceStationPlayerController::OnTogglePause);
		EIC->BindAction(SpeedUpAction, ETriggerEvent::Started, this, &ASpaceStationPlayerController::OnSpeedUp);
		EIC->BindAction(SpeedDownAction, ETriggerEvent::Started, this, &ASpaceStationPlayerController::OnSpeedDown);
	}
}

void ASpaceStationPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Cache the controlled pawn
	ControlledPawn = Cast<ASpaceStationPawn>(InPawn);

	// Get HUD reference
	SpaceStationHUD = Cast<ASpaceStationHUD>(GetHUD());

	// Get station grid from game mode
	if (ASpaceStationGameMode* GM = GetWorld()->GetAuthGameMode<ASpaceStationGameMode>())
	{
		StationGrid = GM->GetStationGrid();
	}
}

void ASpaceStationPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Auto-enter build mode for testing
	if (bAutoEnterBuildMode && DefaultBuildModuleClass)
	{
		EnterBuildMode(DefaultBuildModuleClass);
	}
}

void ASpaceStationPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Lazy-initialize StationGrid (OnPossess runs before GameMode::BeginPlay)
	if (!StationGrid)
	{
		if (ASpaceStationGameMode* GM = GetWorld()->GetAuthGameMode<ASpaceStationGameMode>())
		{
			StationGrid = GM->GetStationGrid();
		}
	}

	// Update build preview if in build mode
	if (bInBuildMode)
	{
		UpdateBuildPreview();
	}

	// Update delete mode highlight
	if (bInDeleteMode)
	{
		UpdateDeletePreview();
	}

	// Edge scrolling
	if (bEnableEdgeScrolling)
	{
		UpdateEdgeScrolling(DeltaSeconds);
	}
}

void ASpaceStationPlayerController::EnterBuildMode(TSubclassOf<AStationModule> ModuleClass)
{
	if (!ModuleClass)
	{
		return;
	}

	bInBuildMode = true;
	SelectedModuleClass = ModuleClass;
	PreviewRotation = FRotator::ZeroRotator;

	// Spawn preview module
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PreviewModule = GetWorld()->SpawnActor<AStationModule>(ModuleClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (PreviewModule)
	{
		PreviewModule->SetPreviewMode(true);
	}
}

void ASpaceStationPlayerController::ExitBuildMode()
{
	bInBuildMode = false;
	SelectedModuleClass = nullptr;
	PreviewRotation = FRotator::ZeroRotator;

	// Destroy preview module
	if (PreviewModule)
	{
		PreviewModule->Destroy();
		PreviewModule = nullptr;
	}
}

void ASpaceStationPlayerController::UpdateBuildPreview()
{
	if (!PreviewModule || !StationGrid)
	{
		return;
	}

	FIntPoint GridCoord;
	if (GetGridLocationUnderCursor(GridCoord))
	{
		FVector WorldPos = StationGrid->GridToWorld(GridCoord);
		PreviewModule->SetActorLocation(WorldPos);
		PreviewModule->SetActorRotation(PreviewRotation);

		bool bValid = StationGrid->IsValidPlacement(PreviewModule, GridCoord, PreviewRotation);

		// Also check if player can afford this module
		if (bValid)
		{
			if (ASpaceStationGameMode* GM = GetWorld()->GetAuthGameMode<ASpaceStationGameMode>())
			{
				bValid = GM->CanAffordModule(PreviewModule->BuildCost);
			}
		}

		PreviewModule->SetValidPlacement(bValid);
	}
}

void ASpaceStationPlayerController::PlaceModule()
{
	if (!PreviewModule || !StationGrid)
		return;

	FIntPoint GridCoord;
	if (GetGridLocationUnderCursor(GridCoord))
	{
		// Check if valid placement
		if (StationGrid->IsValidPlacement(PreviewModule, GridCoord, PreviewRotation))
		{
			// Check cost
			ASpaceStationGameMode* GM = GetWorld()->GetAuthGameMode<ASpaceStationGameMode>();
			if (GM && !GM->CanAffordModule(PreviewModule->BuildCost))
				return;

			// Place the module
			if (StationGrid->PlaceModule(PreviewModule, GridCoord, PreviewRotation))
			{
				// Pay for the module and register
				if (GM)
				{
					GM->PayForModule(PreviewModule->BuildCost);
					GM->RegisterModule(PreviewModule);
				}

				// Set preview mode off (now a real module)
				PreviewModule->SetPreviewMode(false);

				// Clear preview and spawn a new one for continuous building
				PreviewModule = nullptr;

				// Spawn new preview for next placement
				if (SelectedModuleClass)
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					PreviewModule = GetWorld()->SpawnActor<AStationModule>(SelectedModuleClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
					if (PreviewModule)
					{
						PreviewModule->SetPreviewMode(true);
					}
				}
			}
		}
	}
}

void ASpaceStationPlayerController::RotatePreview()
{
	// Rotate by 90 degrees
	float NewYaw = PreviewRotation.Yaw + 90.0f;
	if (NewYaw >= 360.0f)
		NewYaw -= 360.0f;
	PreviewRotation = FRotator(0.0f, NewYaw, 0.0f);
}

bool ASpaceStationPlayerController::GetGridLocationUnderCursor(FIntPoint& OutGridCoord)
{
	if (!StationGrid)
		return false;

	FHitResult HitResult;
	bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	if (bHit)
	{
		OutGridCoord = StationGrid->WorldToGrid(HitResult.Location);
		return true;
	}

	return false;
}

void ASpaceStationPlayerController::SelectCrew(ACrewMember* Crew, bool bAdditive)
{
	if (!Crew)
		return;

	if (bAdditive)
	{
		// Shift+click: toggle this crew in selection
		if (SelectedCrew.Contains(Crew))
		{
			Crew->SetSelected(false);
			SelectedCrew.Remove(Crew);
		}
		else
		{
			Crew->SetSelected(true);
			SelectedCrew.Add(Crew);
		}
	}
	else
	{
		// Normal click: clear previous selection
		DeselectAllCrew();

		// Select new crew
		Crew->SetSelected(true);
		SelectedCrew.Add(Crew);
	}
}

void ASpaceStationPlayerController::DeselectAllCrew()
{
	for (ACrewMember* Selected : SelectedCrew)
	{
		if (Selected)
		{
			Selected->SetSelected(false);
		}
	}
	SelectedCrew.Empty();
}

void ASpaceStationPlayerController::CommandCrewMove(const FVector& Location)
{
	for (ACrewMember* Crew : SelectedCrew)
	{
		if (Crew)
		{
			// Command via AI controller
			ACrewAIController* AIC = Cast<ACrewAIController>(Crew->GetController());
			if (AIC)
			{
				AIC->CommandMoveToLocation(Location);
			}
		}
	}
}

// Input Handlers

void ASpaceStationPlayerController::CameraPan(const FInputActionValue& Value)
{
	FVector2D PanInput = Value.Get<FVector2D>();

	if (ControlledPawn)
	{
		// Convert 2D input to world space movement
		FVector Forward = FVector::ForwardVector;
		FVector Right = FVector::RightVector;

		FVector Movement = (Forward * PanInput.Y) + (Right * PanInput.X);
		Movement.Normalize();

		ControlledPawn->AddMovementInput(Movement);
	}
}

void ASpaceStationPlayerController::CameraZoom(const FInputActionValue& Value)
{
	float ZoomDelta = Value.Get<float>();

	if (ControlledPawn)
	{
		float CurrentZoom = ControlledPawn->GetCurrentZoom();
		float NewZoom = CurrentZoom + (ZoomDelta * ZoomSpeed);
		ControlledPawn->SetZoomLevel(NewZoom);
	}
}

void ASpaceStationPlayerController::OnResetCamera(const FInputActionValue& Value)
{
	// Reset camera to default position/zoom
	if (ControlledPawn)
	{
		ControlledPawn->SetActorLocation(FVector::ZeroVector);
		ControlledPawn->SetZoomLevel(2500.0f);
	}
}

void ASpaceStationPlayerController::OnSelect(const FInputActionValue& Value)
{
	// In delete mode, left click deletes module under cursor
	if (bInDeleteMode)
	{
		DeleteModuleUnderCursor();
		return;
	}

	// Handle selection (crew) when not in build mode
	if (!bInBuildMode)
	{
		bool bShiftHeld = IsInputKeyDown(EKeys::LeftShift) || IsInputKeyDown(EKeys::RightShift);

		FHitResult HitResult;
		if (GetHitResultUnderCursor(ECC_Pawn, false, HitResult))
		{
			ACrewMember* HitCrew = Cast<ACrewMember>(HitResult.GetActor());
			if (HitCrew)
			{
				SelectCrew(HitCrew, bShiftHeld);
				return;
			}
		}

		// Clicked on nothing - deselect all (unless shift held)
		if (!bShiftHeld)
		{
			DeselectAllCrew();
		}
	}
}

void ASpaceStationPlayerController::OnPlaceModule(const FInputActionValue& Value)
{
	if (bInBuildMode)
	{
		PlaceModule();
	}
}

void ASpaceStationPlayerController::OnCancelPlacement(const FInputActionValue& Value)
{
	if (bInDeleteMode)
	{
		ExitDeleteMode();
	}
	else if (bInBuildMode)
	{
		ExitBuildMode();
	}
}

void ASpaceStationPlayerController::OnRotatePreview(const FInputActionValue& Value)
{
	if (bInBuildMode)
	{
		RotatePreview();
	}
}

void ASpaceStationPlayerController::OnCommandCrew(const FInputActionValue& Value)
{
	// Right-click: command selected crew to move (only when not in build mode)
	if (bInBuildMode)
		return;

	if (SelectedCrew.Num() == 0)
		return;

	FHitResult HitResult;
	if (GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		CommandCrewMove(HitResult.Location);
	}
}

void ASpaceStationPlayerController::OnOpenBuildMenu(const FInputActionValue& Value)
{
	// Toggle build menu via HUD
	if (!SpaceStationHUD)
	{
		SpaceStationHUD = Cast<ASpaceStationHUD>(GetHUD());
	}

	if (SpaceStationHUD)
	{
		SpaceStationHUD->ToggleBuildMenu();
	}
}

void ASpaceStationPlayerController::OnDeleteModule(const FInputActionValue& Value)
{
	if (bInDeleteMode)
	{
		// In delete mode: click to delete module under cursor
		DeleteModuleUnderCursor();
	}
	else
	{
		// Toggle into delete mode (exit build mode first if active)
		if (bInBuildMode)
		{
			ExitBuildMode();
		}
		EnterDeleteMode();
	}
}

void ASpaceStationPlayerController::EnterDeleteMode()
{
	bInDeleteMode = true;
	// Exit build mode if active
	if (bInBuildMode)
	{
		ExitBuildMode();
	}
}

void ASpaceStationPlayerController::ExitDeleteMode()
{
	bInDeleteMode = false;

	// Clear highlight on previous module
	if (HighlightedModule)
	{
		HighlightedModule->SetValidPlacement(true); // Reset visual
		HighlightedModule = nullptr;
	}
}

void ASpaceStationPlayerController::DeleteModuleUnderCursor()
{
	if (!StationGrid)
		return;

	FIntPoint GridCoord;
	if (GetGridLocationUnderCursor(GridCoord))
	{
		AStationModule* Module = StationGrid->GetModuleAt(GridCoord);
		if (Module && Module->bIsPlaced)
		{
			if (Module == HighlightedModule)
			{
				HighlightedModule = nullptr;
			}

			// Refund half the build cost
			if (ASpaceStationGameMode* GM = GetWorld()->GetAuthGameMode<ASpaceStationGameMode>())
			{
				GM->AddCredits(Module->BuildCost / 2);
			}

			StationGrid->RemoveModule(Module->GridPosition);
		}
	}
}

void ASpaceStationPlayerController::OnSpawnCrew(const FInputActionValue& Value)
{
	SpawnCrewAtCursor();
}

void ASpaceStationPlayerController::OnTogglePause(const FInputActionValue& Value)
{
	TogglePause();
}

void ASpaceStationPlayerController::OnSpeedUp(const FInputActionValue& Value)
{
	if (GameSpeed < 1.0f)
		SetGameSpeed(1.0f);
	else if (GameSpeed < 2.0f)
		SetGameSpeed(2.0f);
	else if (GameSpeed < 4.0f)
		SetGameSpeed(4.0f);
}

void ASpaceStationPlayerController::OnSpeedDown(const FInputActionValue& Value)
{
	if (GameSpeed > 2.0f)
		SetGameSpeed(2.0f);
	else if (GameSpeed > 1.0f)
		SetGameSpeed(1.0f);
	else if (GameSpeed > 0.5f)
		SetGameSpeed(0.5f);
}

void ASpaceStationPlayerController::SpawnCrewAtCursor()
{
	ASpaceStationGameMode* GM = GetWorld()->GetAuthGameMode<ASpaceStationGameMode>();
	if (!GM)
		return;

	FHitResult HitResult;
	if (GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		FVector CrewSpawnPos = HitResult.Location;
		CrewSpawnPos.Z += 100.0f; // Spawn slightly above ground
		GM->SpawnCrewMember(CrewSpawnPos);
	}
}

void ASpaceStationPlayerController::TogglePause()
{
	bGamePaused = !bGamePaused;

	if (UWorld* World = GetWorld())
	{
		if (bGamePaused)
		{
			World->GetWorldSettings()->SetTimeDilation(0.001f); // Near-zero, not true pause (keeps UI responsive)
		}
		else
		{
			World->GetWorldSettings()->SetTimeDilation(GameSpeed);
		}
	}
}

void ASpaceStationPlayerController::SetGameSpeed(float Speed)
{
	GameSpeed = FMath::Clamp(Speed, 0.5f, 4.0f);

	if (!bGamePaused)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetWorldSettings()->SetTimeDilation(GameSpeed);
		}
	}
}

void ASpaceStationPlayerController::UpdateEdgeScrolling(float DeltaSeconds)
{
	if (!ControlledPawn)
		return;

	float MouseX, MouseY;
	if (!GetMousePosition(MouseX, MouseY))
		return;

	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);

	FVector2D ScrollDirection = FVector2D::ZeroVector;

	// Check screen edges
	if (MouseX <= EdgeScrollThreshold)
	{
		ScrollDirection.X = -1.0f * (1.0f - MouseX / EdgeScrollThreshold); // Stronger near edge
	}
	else if (MouseX >= ViewportSizeX - EdgeScrollThreshold)
	{
		ScrollDirection.X = 1.0f * (1.0f - (ViewportSizeX - MouseX) / EdgeScrollThreshold);
	}

	if (MouseY <= EdgeScrollThreshold)
	{
		ScrollDirection.Y = 1.0f * (1.0f - MouseY / EdgeScrollThreshold);
	}
	else if (MouseY >= ViewportSizeY - EdgeScrollThreshold)
	{
		ScrollDirection.Y = -1.0f * (1.0f - (ViewportSizeY - MouseY) / EdgeScrollThreshold);
	}

	if (!ScrollDirection.IsNearlyZero())
	{
		FVector Movement = (FVector::ForwardVector * ScrollDirection.Y) + (FVector::RightVector * ScrollDirection.X);
		Movement *= EdgeScrollSpeed * DeltaSeconds;
		ControlledPawn->AddActorWorldOffset(Movement);
	}
}

void ASpaceStationPlayerController::UpdateDeletePreview()
{
	if (!StationGrid)
		return;

	FIntPoint GridCoord;
	AStationModule* ModuleUnderCursor = nullptr;

	if (GetGridLocationUnderCursor(GridCoord))
	{
		ModuleUnderCursor = StationGrid->GetModuleAt(GridCoord);
		if (ModuleUnderCursor && !ModuleUnderCursor->bIsPlaced)
		{
			ModuleUnderCursor = nullptr;
		}
	}

	// Update highlight
	if (ModuleUnderCursor != HighlightedModule)
	{
		// Clear old highlight
		if (HighlightedModule)
		{
			HighlightedModule->SetValidPlacement(true); // Reset to normal
		}

		// Set new highlight (red tint = about to delete)
		HighlightedModule = ModuleUnderCursor;
		if (HighlightedModule)
		{
			HighlightedModule->SetValidPlacement(false); // Red tint
		}
	}
}
