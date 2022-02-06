// Fill out your copyright notice in the Description page of Project Settings.


#include "Action/BlueprintAsyncTask.h"

FTimerHandle UBlueprintAsyncTask::TimerHandle;
bool UBlueprintAsyncTask::bFinished;

void UBlueprintAsyncTask::CheckCompletion()
{
	if (bFinished)
	{
		if (UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr)
		{
			bFinished = false;

			World->GetTimerManager().ClearTimer(TimerHandle);
			TimerHandle.Invalidate();

			UE_LOG(LogTemp, Display, L"Async Task Completed");
			Completed.Broadcast();
		}
	}
}

UBlueprintAsyncTask* UBlueprintAsyncTask::RunAsyncTask(UObject* WorldContextObject, const FAsyncTaskDelegate& TaskToExecute)
{
	if (auto Task = NewObject<UBlueprintAsyncTask>())
	{
		Task->WorldContext = WorldContextObject;
		Task->TaskToRun = TaskToExecute;
		return Task;
	}

	return nullptr;
}

void UBlueprintAsyncTask::Activate()
{
	Super::Activate();

	if (TimerHandle.IsValid()) return;

	if (UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr)
	{
		AsyncTask(ENamedThreads::AnyThread, [this]
		{
			FScopeLock Lock(&Mutex);
			
			TaskToRun.Execute();
			bFinished = true;
		});

		World->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &UBlueprintAsyncTask::CheckCompletion), 1/60.f, true);
	}
}

void UBlueprintAsyncTask::BeginDestroy()
{
	Super::BeginDestroy();
}
