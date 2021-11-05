// Fill out your copyright notice in the Description page of Project Settings.


#include "ExecuteAsync.h"

class FDBExecuteTask : public FNonAbandonableTask
{
	friend class FAsyncTask<FDBExecuteTask>;

	const bool bTransaction;
	FDbConnectionHandle InDB;
	FDbStatement InStatement;
	FString InQuery;
public:

	FDBExecuteTask(const FDbConnectionHandle& DB, const FString& Query)
		: bTransaction(false)
		, InDB(DB)
		, InStatement()
		, InQuery(Query)
	{
	}

	FDBExecuteTask(const FDbConnectionHandle& DB)
	: bTransaction(false)
	, InDB(DB)
	, InStatement()
	{
	}

	explicit FDBExecuteTask(const FDbStatement& Statement)
		: bTransaction(false)
		, InDB()
		, InStatement(Statement)
	{
	}


	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FGenericTask, STATGROUP_TaskGraphTasks);
	}
	
	void DoWork() 
	{
		if (bTransaction && InDB.IsValidTransaction())
		{
			InDB.CommitTransaction();
		}
		else
		{
			if (InDB.IsValid())
				InDB.Execute(InQuery);
			else
				InStatement.Execute();
		}
	}
};


static TPair<UExecuteAsync*, TUniquePtr<FAsyncTask<FDBExecuteTask>>> TaskPtr;

void UExecuteAsync::CheckCompletion()
{
	if (TaskPtr.Value && TaskPtr.Value->IsDone())
	{
		if (auto World = GEngine->GetWorldFromContextObjectChecked(InWorldContextObject))
		{
			TaskPtr.Value.Release();
			TaskPtr.Key = nullptr;
			World->GetTimerManager().ClearTimer(CompletionCheckHandle);
			CompletionCheckHandle.Invalidate();

			Completed.Broadcast();
		}
	}
}

UExecuteAsync* UExecuteAsync::ExecuteAsync_Database(UObject* WorldContextObject, const FDbConnectionHandle& Database, const FString& Query)
{
	if (UExecuteAsync* Node = NewObject<UExecuteAsync>())
	{
		Node->InWorldContextObject = WorldContextObject;
		Node->InQuery = Query;
		Node->InDb = Database;	
		return Node;
	}

	return nullptr;
}

UExecuteAsync* UExecuteAsync::CommitTransactionAsync(UObject* WorldContextObject, const FDbConnectionHandle& Database)
{
	if (UExecuteAsync* Node = NewObject<UExecuteAsync>())
	{
		Node->InWorldContextObject = WorldContextObject;
		Node->InDb = Database;	
		return Node;
	}

	return nullptr;
}

UExecuteAsync* UExecuteAsync::ExecuteAsync_Statement(UObject* WorldContextObject, const FDbStatement& Statement)
{
	if (UExecuteAsync* Node = NewObject<UExecuteAsync>())
	{
		Node->InWorldContextObject = WorldContextObject;
		Node->InStatement = Statement;	
		return Node;
	}

	return nullptr;
}

void UExecuteAsync::Activate()
{
	if (!TaskPtr.Value)
	{
		if (auto World = GEngine->GetWorldFromContextObjectChecked(InWorldContextObject))
		{
			if (InDb.IsValid())
			{
				if (InQuery.IsEmpty())
					TaskPtr.Value.Reset(new FAsyncTask<FDBExecuteTask>(InDb));
				else
					TaskPtr.Value.Reset(new FAsyncTask<FDBExecuteTask>(InDb, InQuery));
			}
			else
			{
				TaskPtr.Value.Reset(new FAsyncTask<FDBExecuteTask>(InStatement));
			}

			TaskPtr.Value->StartBackgroundTask();
			TaskPtr.Key = this;

			World->GetTimerManager().SetTimer(CompletionCheckHandle, FTimerDelegate::CreateUObject(this, &UExecuteAsync::CheckCompletion), 1/30.f, true);
		}
	}
}

void UExecuteAsync::BeginDestroy()
{
	Super::BeginDestroy();
	
	if (TaskPtr.Value && TaskPtr.Key == this)
	{
		TaskPtr.Value->Cancel();
	}
}
