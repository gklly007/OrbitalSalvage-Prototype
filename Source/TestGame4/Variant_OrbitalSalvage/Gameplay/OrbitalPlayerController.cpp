// Copyright Epic Games, Inc. All Rights Reserved.

#include "OrbitalPlayerController.h"
#include "OrbitalShipPawn.h"
#include "OrbitalGameMode.h"
#include "OrbitalStationActor.h"
#include "OrbitalJumpGateActor.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputModifiers.h"
#include "InputMappingContext.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

AOrbitalPlayerController::AOrbitalPlayerController()
{
	bShowMouseCursor = true;
}

void AOrbitalPlayerController::BeginPlay()
{
	Super::BeginPlay();
	OrbitalGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AOrbitalGameMode>() : nullptr;
}

void AOrbitalPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ControlledShip = Cast<AOrbitalShipPawn>(InPawn);
}

void AOrbitalPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	EnsureRuntimeInputSetup();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOrbitalPlayerController::MoveInput);
		EIC->BindAction(MoveAction, ETriggerEvent::Completed, this, &AOrbitalPlayerController::MoveInput);
		EIC->BindAction(TurnAction, ETriggerEvent::Triggered, this, &AOrbitalPlayerController::TurnInput);
		EIC->BindAction(TurnAction, ETriggerEvent::Completed, this, &AOrbitalPlayerController::TurnInput);
		EIC->BindAction(MineAction, ETriggerEvent::Started, this, &AOrbitalPlayerController::MineStarted);
		EIC->BindAction(MineAction, ETriggerEvent::Completed, this, &AOrbitalPlayerController::MineCompleted);
		EIC->BindAction(MineAction, ETriggerEvent::Canceled, this, &AOrbitalPlayerController::MineCompleted);
		EIC->BindAction(BrakeAction, ETriggerEvent::Started, this, &AOrbitalPlayerController::BrakeStarted);
		EIC->BindAction(BrakeAction, ETriggerEvent::Completed, this, &AOrbitalPlayerController::BrakeCompleted);
		EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &AOrbitalPlayerController::InteractPressed);
		EIC->BindAction(ToggleAssistAction, ETriggerEvent::Started, this, &AOrbitalPlayerController::ToggleAssistPressed);
		EIC->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AOrbitalPlayerController::ZoomInput);
		EIC->BindAction(UpgradeReactorAction, ETriggerEvent::Started, this, &AOrbitalPlayerController::UpgradeReactorPressed);
		EIC->BindAction(UpgradeCargoAction, ETriggerEvent::Started, this, &AOrbitalPlayerController::UpgradeCargoPressed);
		EIC->BindAction(UpgradeMiningAction, ETriggerEvent::Started, this, &AOrbitalPlayerController::UpgradeMiningPressed);
	}
}

void AOrbitalPlayerController::MoveInput(const FInputActionValue& Value)
{
	if (ControlledShip)
	{
		ControlledShip->SetMoveInput(Value.Get<FVector2D>());
	}
}

void AOrbitalPlayerController::TurnInput(const FInputActionValue& Value)
{
	if (ControlledShip)
	{
		ControlledShip->SetTurnInput(Value.Get<float>());
	}
}

void AOrbitalPlayerController::MineStarted(const FInputActionValue& Value)
{
	if (ControlledShip)
	{
		ControlledShip->SetMiningActive(true);
	}
}

void AOrbitalPlayerController::MineCompleted(const FInputActionValue& Value)
{
	if (ControlledShip)
	{
		ControlledShip->SetMiningActive(false);
	}
}

void AOrbitalPlayerController::BrakeStarted(const FInputActionValue& Value)
{
	if (ControlledShip)
	{
		ControlledShip->SetBrakeInput(true);
	}
}

void AOrbitalPlayerController::BrakeCompleted(const FInputActionValue& Value)
{
	if (ControlledShip)
	{
		ControlledShip->SetBrakeInput(false);
	}
}

void AOrbitalPlayerController::InteractPressed(const FInputActionValue& Value)
{
	if (!ControlledShip)
	{
		return;
	}

	if (!OrbitalGameMode)
	{
		OrbitalGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AOrbitalGameMode>() : nullptr;
		if (!OrbitalGameMode)
		{
			return;
		}
	}

	// Try dock interaction first.
	TArray<AActor*> Stations;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrbitalStationActor::StaticClass(), Stations);
	float BestDockDistSq = FLT_MAX;
	AOrbitalStationActor* BestStation = nullptr;

	for (AActor* Candidate : Stations)
	{
		AOrbitalStationActor* Station = Cast<AOrbitalStationActor>(Candidate);
		if (!Station || !Station->CanDockShip(ControlledShip))
		{
			continue;
		}

		const float DistSq = FVector::DistSquared2D(ControlledShip->GetActorLocation(), Station->GetActorLocation());
		if (DistSq < BestDockDistSq)
		{
			BestDockDistSq = DistSq;
			BestStation = Station;
		}
	}

	if (BestStation)
	{
		OrbitalGameMode->HandleDockRequest(BestStation, ControlledShip);
		return;
	}

	// Then try jump gate interaction.
	TArray<AActor*> Gates;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrbitalJumpGateActor::StaticClass(), Gates);
	for (AActor* Candidate : Gates)
	{
		if (AOrbitalJumpGateActor* Gate = Cast<AOrbitalJumpGateActor>(Candidate))
		{
			if (Gate->CanShipUseGate(ControlledShip))
			{
				OrbitalGameMode->HandleJumpGateRequest(Gate, ControlledShip);
				return;
			}
		}
	}
}

void AOrbitalPlayerController::ToggleAssistPressed(const FInputActionValue& Value)
{
	if (ControlledShip)
	{
		ControlledShip->ToggleFlightAssist();
	}
}

void AOrbitalPlayerController::ZoomInput(const FInputActionValue& Value)
{
	if (!ControlledShip || !ControlledShip->SpringArm)
	{
		return;
	}

	const float Delta = Value.Get<float>();
	const float NewArm = FMath::Clamp(ControlledShip->SpringArm->TargetArmLength - Delta * CameraZoomSpeed, 1100.0f, 4200.0f);
	ControlledShip->SpringArm->TargetArmLength = NewArm;
}

void AOrbitalPlayerController::UpgradeReactorPressed(const FInputActionValue& Value)
{
	if (OrbitalGameMode)
	{
		OrbitalGameMode->TryPurchaseUpgrade(EOrbitalUpgradeType::Reactor);
	}
}

void AOrbitalPlayerController::UpgradeCargoPressed(const FInputActionValue& Value)
{
	if (OrbitalGameMode)
	{
		OrbitalGameMode->TryPurchaseUpgrade(EOrbitalUpgradeType::CargoPod);
	}
}

void AOrbitalPlayerController::UpgradeMiningPressed(const FInputActionValue& Value)
{
	if (OrbitalGameMode)
	{
		OrbitalGameMode->TryPurchaseUpgrade(EOrbitalUpgradeType::MiningLaser);
	}
}

void AOrbitalPlayerController::EnsureRuntimeInputSetup()
{
	if (!MoveAction)
	{
		MoveAction = NewObject<UInputAction>(this, TEXT("IA_Orbital_Move"));
		MoveAction->ValueType = EInputActionValueType::Axis2D;
	}
	if (!TurnAction)
	{
		TurnAction = NewObject<UInputAction>(this, TEXT("IA_Orbital_Turn"));
		TurnAction->ValueType = EInputActionValueType::Axis1D;
	}
	if (!MineAction)
	{
		MineAction = NewObject<UInputAction>(this, TEXT("IA_Orbital_Mine"));
		MineAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!BrakeAction)
	{
		BrakeAction = NewObject<UInputAction>(this, TEXT("IA_Orbital_Brake"));
		BrakeAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!InteractAction)
	{
		InteractAction = NewObject<UInputAction>(this, TEXT("IA_Orbital_Interact"));
		InteractAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!ToggleAssistAction)
	{
		ToggleAssistAction = NewObject<UInputAction>(this, TEXT("IA_Orbital_ToggleAssist"));
		ToggleAssistAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!ZoomAction)
	{
		ZoomAction = NewObject<UInputAction>(this, TEXT("IA_Orbital_Zoom"));
		ZoomAction->ValueType = EInputActionValueType::Axis1D;
	}
	if (!UpgradeReactorAction)
	{
		UpgradeReactorAction = NewObject<UInputAction>(this, TEXT("IA_Orbital_UpgradeReactor"));
		UpgradeReactorAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!UpgradeCargoAction)
	{
		UpgradeCargoAction = NewObject<UInputAction>(this, TEXT("IA_Orbital_UpgradeCargo"));
		UpgradeCargoAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!UpgradeMiningAction)
	{
		UpgradeMiningAction = NewObject<UInputAction>(this, TEXT("IA_Orbital_UpgradeMining"));
		UpgradeMiningAction->ValueType = EInputActionValueType::Boolean;
	}

	if (!FlightMappingContext)
	{
		FlightMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Orbital_Runtime"));
	}

	FlightMappingContext->UnmapAll();

	// Move (WASD as Axis2D)
	FEnhancedActionKeyMapping& WMap = FlightMappingContext->MapKey(MoveAction, EKeys::W);
	UInputModifierSwizzleAxis* WSwizzle = NewObject<UInputModifierSwizzleAxis>(FlightMappingContext);
	WSwizzle->Order = EInputAxisSwizzle::YXZ;
	WMap.Modifiers.Add(WSwizzle);

	FEnhancedActionKeyMapping& SMap = FlightMappingContext->MapKey(MoveAction, EKeys::S);
	UInputModifierSwizzleAxis* SSwizzle = NewObject<UInputModifierSwizzleAxis>(FlightMappingContext);
	SSwizzle->Order = EInputAxisSwizzle::YXZ;
	UInputModifierNegate* SNegate = NewObject<UInputModifierNegate>(FlightMappingContext);
	SMap.Modifiers.Add(SSwizzle);
	SMap.Modifiers.Add(SNegate);

	FlightMappingContext->MapKey(MoveAction, EKeys::D);

	FEnhancedActionKeyMapping& AMap = FlightMappingContext->MapKey(MoveAction, EKeys::A);
	UInputModifierNegate* ANegate = NewObject<UInputModifierNegate>(FlightMappingContext);
	AMap.Modifiers.Add(ANegate);

	// Turn
	FlightMappingContext->MapKey(TurnAction, EKeys::E);
	FEnhancedActionKeyMapping& QMap = FlightMappingContext->MapKey(TurnAction, EKeys::Q);
	UInputModifierNegate* QNegate = NewObject<UInputModifierNegate>(FlightMappingContext);
	QMap.Modifiers.Add(QNegate);

	// Main interactions
	FlightMappingContext->MapKey(MineAction, EKeys::LeftMouseButton);
	FlightMappingContext->MapKey(BrakeAction, EKeys::SpaceBar);
	FlightMappingContext->MapKey(InteractAction, EKeys::F);
	FlightMappingContext->MapKey(ToggleAssistAction, EKeys::T);
	FlightMappingContext->MapKey(ZoomAction, EKeys::MouseWheelAxis);

	// Upgrades
	FlightMappingContext->MapKey(UpgradeReactorAction, EKeys::One);
	FlightMappingContext->MapKey(UpgradeCargoAction, EKeys::Two);
	FlightMappingContext->MapKey(UpgradeMiningAction, EKeys::Three);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(FlightMappingContext, 0);
	}
}

