// Fill out your copyright notice in the Description page of Project Settings.


#include "SqliteDatabase.h"
#include "SmoothSql.h"
#include "SqliteStatement.h"
#include "SqliteTransaction.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystem/DatabaseSingleton.h"

USqliteDatabase::USqliteDatabase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Database = nullptr;

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		Singleton = GEngine ? GEngine->GetEngineSubsystem<UDatabaseSingleton>() : nullptr;
	}
}

bool USqliteDatabase::InitConnection(const FSqliteDBConnectionParms& Params)
{
	if (!Database)
	{
		// Obtain path to project dir
		const auto GameDir = FPaths::ConvertRelativePathToFull( FPaths::ProjectDir() );
		const auto DBName = Params.DBName;
		const auto Folder = Params.Folder;

		// Make sure that path is valid
		check(!DBName.IsEmpty())
		check(!Folder.IsEmpty())
		
		// Make path to DB
		auto DBDir = FPaths::Combine(GameDir, Folder, DBName);
			 DBDir = FPaths::SetExtension(DBDir, "db");

		// Try to open DB
		try
		{
			Database.Reset(new SQLite::Database(std::string(TCHAR_TO_UTF8(*DBDir)), SQLite::OPEN_READWRITE, Params.BusyTimeout));
			UE_LOG(LogSmoothSqlite, Display, L"Opened database '%s', located '%s'", *DBName, *DBDir);
			return true;
		}
		catch (const SQLite::Exception& e)
		{
			UE_LOG(LogSmoothSqlite, Error, L"%s", *FString(e.getErrorStr()))
			Database.Reset();
			return false;
		}
	}

	return false;
}

void USqliteDatabase::CloseConnection()
{
	if (Database)
	{
		// Release underlaying db
		Database.Release();

		// Notify singleton that connection was destroyed
		Singleton->CloseConnection(this);
	}
}

bool USqliteDatabase::HasValidConnection() const
{
	return Database ? bool ( Database->getHandle() ) : ( false );
}

bool USqliteDatabase::CommitTransaction()
{
	if (CurrentTransaction)
	{
		CurrentTransaction->Commit();

		if (!CurrentTransaction->IsValidLowLevel())
		{
			CurrentTransaction = nullptr;
			return true;
		}
	}

	return false;
}

bool USqliteDatabase::RollbackTransaction()
{
	if (CurrentTransaction)
	{
		CurrentTransaction->Rollback();

		if (!CurrentTransaction->IsValidLowLevel())
		{
			CurrentTransaction = nullptr;
			return true;
		}
	}

	return false;
}

SQLite::Database* USqliteDatabase::GetDatabaseConnection() const
{
	return Database.Get();
}

USqliteStatement* USqliteDatabase::CreateQuery(const FString& QueryString)
{
	if (HasValidConnection())
	{
		// Create statement object
		auto Statement = NewObject<USqliteStatement>(this);

		// If was init without errors, we can return it
		if (Statement->InitQuery(this, QueryString))
		{
			return Statement;	
		}
	}

	return nullptr;
}

USqliteTransaction* USqliteDatabase::BeginTransaction()
{
	if (HasValidConnection() && !CurrentTransaction)
	{
		CurrentTransaction = NewObject<USqliteTransaction>();
		if (CurrentTransaction->InitTransaction(*GetDatabaseConnection()))
		{
			return CurrentTransaction;
		}
		else
		{
			CurrentTransaction->MarkPendingKill();
			CurrentTransaction = nullptr;
		}
	}

	return nullptr;
}

bool USqliteDatabase::HasActiveTransaction() const
{
	return CurrentTransaction != nullptr && CurrentTransaction->IsValidLowLevelFast();
}

USqliteTransaction* USqliteDatabase::GetTransaction() const
{
	return CurrentTransaction;
}

bool USqliteDatabase::Execute(const FString& Query)
{
	if (HasValidConnection())
	{
		try
		{
			Database->exec(std::string(TCHAR_TO_UTF8(*Query)));
			return true;
		}
		catch (SQLite::Exception& e)
		{
			UE_LOG(LogSmoothSqlite, Error, L"Error during Database Execute: %s", *FString(e.getErrorStr()))
		}
	}

	return false;
}

FSqliteColumn USqliteDatabase::Fetch(const FString& Query)
{
	if (HasValidConnection())
	{
		try
		{
			const auto Col = Database->execAndGet(std::string(TCHAR_TO_UTF8(*Query)));
			return FSqliteColumn(Col);
		}
		catch (SQLite::Exception& e)
		{
			UE_LOG(LogSmoothSqlite, Error, L"Error during Database Fetch: %s", *FString(e.getErrorStr()))
		}
	}

	return {};
}


USqliteDatabase::~USqliteDatabase()
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		CloseConnection();
		UE_LOG(LogSmoothSqlite, Display, L"Connection destroy executed")
	}
}

