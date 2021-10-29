// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/DatabaseSingleton.h"

#include "SmoothSql.h"
#include "SqliteDatabase.h"
#include "SqliteTransaction.h"
#include "DeferredDatabaseCleanupAction.h"

bool UDatabaseSingleton::CleanupTick(float InDeltaTime, USqliteDatabase* Db)
{
	GetDefault<UDeferredDatabaseCleanupAction>(CleanupAction)->OnDatabaseCleanup(Db);
	FinalizeDb(Db);
	return false;
}

USqliteDatabase* UDatabaseSingleton::CreateConnection(const FSqliteDBConnectionParms& Parms, TSubclassOf<UDeferredDatabaseCleanupAction> CleanupActionClass) const
{
	if (Parms.DBName.IsEmpty()) return nullptr;
	if (Parms.Folder.IsEmpty()) return nullptr;

	FTicker::GetCoreTicker().RemoveTicker(TickHandle);
		
	const auto MutableThis = const_cast<UDatabaseSingleton*>(this);
	auto CurrentDatabase = NewObject<USqliteDatabase>(MutableThis, USqliteDatabase::StaticClass(), "SqliteDBConnection");
		
	if (CurrentDatabase->InitConnection(Parms))
	{
		CurrentDatabase->AddToRoot();
		Connections.Add(CurrentDatabase, CurrentDatabase);
		CleanupAction = CleanupActionClass.Get();
	}
	else
	{
		CurrentDatabase->CloseConnection();
	}

	return CurrentDatabase;
}


void UDatabaseSingleton::FinalizeDb(USqliteDatabase* const Db)
{
	// And from map
	Connections.Remove(Db);

	
	if (Db->IsValidLowLevelFast())
	{
		// Remove it from root
		Db->RemoveFromRoot();
		Db->MarkPendingKill();
		Db->CloseConnection();
	}
	
	FTicker::GetCoreTicker().RemoveTicker(TickHandle);
	
	UE_LOG(LogSmoothSqlite, Display, L"Destroying database connection...");
}

void UDatabaseSingleton::CloseConnection(UObject* WorldContext, USqliteDatabase* DB)
{
	// Find connection 
	if (const auto RemoveDb = Connections.Find(DB))
	{
		const auto Db = *RemoveDb;
		
		if (CleanupAction)
		{
			TickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UDatabaseSingleton::CleanupTick, Db), 0.5f);
		}
		else
		{
			FinalizeDb(Db);
		}
		
	}
}

void UDatabaseSingleton::Deinitialize()
{
	FTicker::GetCoreTicker().RemoveTicker(TickHandle);
}
