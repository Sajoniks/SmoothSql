// Fill out your copyright notice in the Description page of Project Settings.


#include "ExecuteQueryAsync.h"

#include "DbComponents/DbStmt.h"

bool UExecuteQueryAsync::bActive = false;


void UExecuteQueryAsync::Step()
{
	if (Stmt->Fetch())
	{
		// Broadcast that we got something
		Body.Broadcast();
	}
	else
	{
		// Clear all timers
		auto World = GEngine->GetWorldFromContextObjectChecked(WorldContext);
		World->GetTimerManager().ClearAllTimersForObject(this);
		TimerHandle.Invalidate();

		// No queries active
		Stmt = nullptr;
		bActive = false;
				
		Completed.Broadcast();

		// Notify others that query on given statement finished
		// Singleton->OnAsyncQueryEnd.Broadcast(Statement);
	}
}

UExecuteQueryAsync* UExecuteQueryAsync::ForEachResultAsync(UObject* WorldContextObject, UDbStmt* Statement)
{
	if (UExecuteQueryAsync* Node = NewObject<UExecuteQueryAsync>())
	{
		Node->WorldContext = WorldContextObject;
		Node->Stmt = Statement;
		return Node;
	}

	return nullptr;
}

void UExecuteQueryAsync::Activate()
{
	// Check if any query is already running
	if (bActive)
	{
		FFrame::KismetExecutionMessage(L"Quering is already in progress", ELogVerbosity::Warning);
		return;
	}

	// Start quering
	if (auto World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull))
	{
		bActive = true;
		World->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &UExecuteQueryAsync::Step), 1/60.f, true);
	}
}

void UExecuteQueryAsync::BeginDestroy()
{
	Super::BeginDestroy();

	if (TimerHandle.IsValid())
	{
		TimerHandle.Invalidate();
		bActive = false;
	}
}
