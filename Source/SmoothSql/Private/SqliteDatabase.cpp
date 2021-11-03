// Fill out your copyright notice in the Description page of Project Settings.


#include "SqliteDatabase.h"
#include "SmoothSql.h"
#include "SqliteStatement.h"
#include "Kismet/KismetSystemLibrary.h"

USqliteDatabase::USqliteDatabase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool USqliteDatabase::InitConnection(const FSqliteDBConnectionParms& Params)
{
	if (!HasValidConnection())
	{
		DBParams = Params;
		return true;
	}

	return false;
}

void USqliteDatabase::CloseConnection()
{
	if (HasValidConnection())
	{
		// Release underlaying db
		Database.Release();
		RemoveFromRoot();
	}
}

void USqliteDatabase::OpenConnection()
{
	if (HasValidConnection()) return;
	
	
	// Obtain path to project dir
	const auto GameDir = FPaths::ConvertRelativePathToFull( FPaths::ProjectDir() );
	const auto DBName = DBParams.DBName;
	const auto Folder = DBParams.Folder;

	// Make sure that path is valid
	check(!DBName.IsEmpty())
	check(!Folder.IsEmpty())
		
	// Make path to DB
	auto DBDir = FPaths::Combine(GameDir, Folder, DBName);
	DBDir = FPaths::SetExtension(DBDir, "db");

	// Try to open DB
	try
	{
		Database.Reset(new SQLite::Database(std::string(TCHAR_TO_UTF8(*DBDir)), SQLite::OPEN_READWRITE, DBParams.BusyTimeout));
		AddToRoot();
		UE_LOG(LogSmoothSqlite, Display, L"Opened database '%s', located '%s'", *DBName, *DBDir);
	}
	catch (const SQLite::Exception& e)
	{
		UE_LOG(LogSmoothSqlite, Error, L"Error occured while Open: %s", *FString(e.getErrorStr()))
		Database.Reset();
	}
}

bool USqliteDatabase::HasValidConnection() const
{
	return Database ? bool ( Database->getHandle() ) : ( false );
}

bool USqliteDatabase::CommitTransaction()
{
	if (HasActiveTransaction())
	{
		Transaction->commit();
		Transaction.Reset();
		return true;
	}

	return false;
}

bool USqliteDatabase::RollbackTransaction()
{
	if (HasActiveTransaction())
	{
		Transaction.Reset();
		return true;
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

void USqliteDatabase::BeginTransaction()
{
	if (!HasActiveTransaction() && HasValidConnection())
	{
		Transaction.Reset(new SQLite::Transaction(*GetDatabaseConnection()));
	}
}


bool USqliteDatabase::HasActiveTransaction() const
{
	return Transaction.IsValid();
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

