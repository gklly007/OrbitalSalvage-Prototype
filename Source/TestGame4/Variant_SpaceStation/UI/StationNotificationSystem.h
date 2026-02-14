// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StationNotificationSystem.generated.h"

/**
 * Priority levels for notifications.
 */
UENUM(BlueprintType)
enum class ENotificationPriority : uint8
{
	Info,       // General information
	Warning,    // Something needs attention
	Critical,   // Urgent action required
	Success     // Positive feedback
};

/**
 * A single notification entry.
 */
USTRUCT(BlueprintType)
struct FStationNotification
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Notification")
	FText Message;

	UPROPERTY(BlueprintReadOnly, Category="Notification")
	ENotificationPriority Priority = ENotificationPriority::Info;

	UPROPERTY(BlueprintReadOnly, Category="Notification")
	float Timestamp = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Notification")
	float Duration = 5.0f;

	FStationNotification() {}
	FStationNotification(const FText& InMessage, ENotificationPriority InPriority, float InTimestamp, float InDuration = 5.0f)
		: Message(InMessage), Priority(InPriority), Timestamp(InTimestamp), Duration(InDuration) {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotificationAdded, const FStationNotification&, Notification);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotificationRemoved, int32, Index);

/**
 * Manages gameplay notifications and event log.
 * Attach to GameMode for station-wide notifications.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UStationNotificationSystem : public UActorComponent
{
	GENERATED_BODY()

public:

	UStationNotificationSystem();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Add a notification */
	UFUNCTION(BlueprintCallable, Category="Notifications")
	void AddNotification(const FText& Message, ENotificationPriority Priority = ENotificationPriority::Info, float Duration = 5.0f);

	/** Get all active notifications */
	UFUNCTION(BlueprintPure, Category="Notifications")
	const TArray<FStationNotification>& GetActiveNotifications() const { return ActiveNotifications; }

	/** Get the full event log (all notifications ever) */
	UFUNCTION(BlueprintPure, Category="Notifications")
	const TArray<FStationNotification>& GetEventLog() const { return EventLog; }

	/** Clear all active notifications */
	UFUNCTION(BlueprintCallable, Category="Notifications")
	void ClearAllNotifications();

	/** Events */
	UPROPERTY(BlueprintAssignable, Category="Notifications")
	FOnNotificationAdded OnNotificationAdded;

	UPROPERTY(BlueprintAssignable, Category="Notifications")
	FOnNotificationRemoved OnNotificationRemoved;

protected:

	/** Currently displayed notifications */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Notifications")
	TArray<FStationNotification> ActiveNotifications;

	/** Full event log */
	TArray<FStationNotification> EventLog;

	/** Maximum entries in event log */
	UPROPERTY(EditAnywhere, Category="Notifications")
	int32 MaxEventLogEntries = 100;

	/** Maximum active notifications shown at once */
	UPROPERTY(EditAnywhere, Category="Notifications")
	int32 MaxActiveNotifications = 5;
};
