// Copyright Epic Games, Inc. All Rights Reserved.

#include "StationNotificationSystem.h"
#include "Engine/World.h"

UStationNotificationSystem::UStationNotificationSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.25f; // Check every 0.25s
}

void UStationNotificationSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetWorld())
		return;

	float CurrentTime = GetWorld()->GetTimeSeconds();

	// Remove expired notifications (iterate backwards for safe removal)
	for (int32 i = ActiveNotifications.Num() - 1; i >= 0; --i)
	{
		if (CurrentTime - ActiveNotifications[i].Timestamp >= ActiveNotifications[i].Duration)
		{
			ActiveNotifications.RemoveAt(i);
			OnNotificationRemoved.Broadcast(i);
		}
	}
}

void UStationNotificationSystem::AddNotification(const FText& Message, ENotificationPriority Priority, float Duration)
{
	if (!GetWorld())
		return;

	float CurrentTime = GetWorld()->GetTimeSeconds();
	FStationNotification NewNotification(Message, Priority, CurrentTime, Duration);

	// Add to event log
	EventLog.Add(NewNotification);
	if (EventLog.Num() > MaxEventLogEntries)
	{
		EventLog.RemoveAt(0);
	}

	// Add to active notifications
	ActiveNotifications.Add(NewNotification);
	if (ActiveNotifications.Num() > MaxActiveNotifications)
	{
		ActiveNotifications.RemoveAt(0);
		OnNotificationRemoved.Broadcast(0);
	}

	OnNotificationAdded.Broadcast(NewNotification);
}

void UStationNotificationSystem::ClearAllNotifications()
{
	ActiveNotifications.Empty();
}
