// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OrbitalTypes.h"
#include "MissionComponent.generated.h"

/**
 * Tracks the prototype mission flow:
 * 1) Recover black box in Belt
 * 2) Jump to Ruins
 * 3) Extract at beacon
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UMissionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMissionComponent();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mission")
	EOrbitalMissionStage CurrentStage = EOrbitalMissionStage::RetrieveBlackBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mission")
	bool bHasRecoveredBlackBox = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
	int32 CompletionReward = 4000;

	UFUNCTION(BlueprintPure, Category="Mission")
	FText GetObjectiveText() const;

	UFUNCTION(BlueprintCallable, Category="Mission")
	void MarkBlackBoxRecovered();

	UFUNCTION(BlueprintCallable, Category="Mission")
	void NotifyEnteredSector(EOrbitalSectorId Sector);

	UFUNCTION(BlueprintCallable, Category="Mission")
	bool TryCompleteExtraction(int32& OutReward);

	UFUNCTION(BlueprintPure, Category="Mission")
	bool IsMissionComplete() const { return CurrentStage == EOrbitalMissionStage::Completed; }
};

