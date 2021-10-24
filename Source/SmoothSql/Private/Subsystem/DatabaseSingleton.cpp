// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/DatabaseSingleton.h"

#include "SmoothSql.h"
#include "SqliteDatabase.h"
#include "SqliteTransaction.h"


USqliteDatabase* UDatabaseSingleton::CreateConnection(const FSqliteDBConnectionParms& Parms) const
{
	if (Parms.DBName.IsEmpty()) return nullptr;
	if (Parms.Folder.IsEmpty()) return nullptr;
		
	const auto MutableThis = const_cast<UDatabaseSingleton*>(this);
	auto CurrentDatabase = NewObject<USqliteDatabase>(MutableThis, USqliteDatabase::StaticClass(), "SqliteDBConnection");
		
	if (CurrentDatabase->InitConnection(Parms))
	{
		CurrentDatabase->AddToRoot();
		Connections.Add(CurrentDatabase, CurrentDatabase);
	}
	else
	{
		CurrentDatabase->CloseConnection();
	}

	return CurrentDatabase;
}

USqliteTransaction* UDatabaseSingleton::StartTransaction(USqliteDatabase* Connection)
{

}


void UDatabaseSingleton::CloseConnection(USqliteDatabase* DB)
{
	// Find connection 
	if (const auto RemoveDb = Connections.Find(DB))
	{
		const auto Db = *RemoveDb;

		if (Db->IsValidLowLevelFast())
		{
			// Remove it from root
			Db->RemoveFromRoot();
			Db->MarkPendingKill();
		}


		// And from map
		Connections.Remove(Db);
		
		UE_LOG(LogSmoothSqlite, Display, L"Destroying database connection...");
	}
}

void UDatabaseSingleton::Deinitialize()
{
	
}
