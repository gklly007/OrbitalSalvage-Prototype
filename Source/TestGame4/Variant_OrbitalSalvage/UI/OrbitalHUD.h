// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OrbitalHUD.generated.h"

/**
 * Canvas HUD for fast prototype feedback and controls.
 */
UCLASS()
class AOrbitalHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
};

