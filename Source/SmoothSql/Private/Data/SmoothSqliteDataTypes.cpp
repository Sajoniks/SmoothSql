// Fill out your copyright notice in the Description page of Project Settings.
#include "Data/SmoothSqliteDataTypes.h"
#include "SmoothSql.h"
#include "SQLiteCpp/Transaction.h"

#define SQLITE_TRY \
	try { \

#define SQLITE_TEXT(val) #val

#define SQLITE_CATCH(action) \
	} catch (SQLite::Exception& e) { \
		UE_LOG(LogSmoothSqlite, Error, TEXT(SQLITE_TEXT(Error occured while action: %s)), *FString(e.getErrorStr())) \
	}


FDbConnectionHandle::FDbConnectionHandle(const FSqliteDBConnectionParms& Params)
{
		ConnectionParms = Params;
	
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
		SQLITE_TRY
			ConnectionPtr = MakeShared<SQLite::Database>(std::string(TCHAR_TO_UTF8(*DBDir)), SQLite::OPEN_READWRITE, Params.BusyTimeout);
		SQLITE_CATCH(Connection Opening)
}

bool FDbConnectionHandle::BeginTransaction()
{
	SQLITE_TRY
		if (IsValid())
		{
			TransactionPtr = MakeShared<SQLite::Transaction>(*ConnectionPtr, SQLite::TransactionBehavior::IMMEDIATE);
			return true;
		}
	SQLITE_CATCH(Begin Transaction)

	return false;
}

bool FDbConnectionHandle::CommitTransaction()
{
	SQLITE_TRY
		if (IsValidTransaction())
		{
			TransactionPtr->commit();
			TransactionPtr.Reset();
			return true;
		}
	SQLITE_CATCH(Commit)
	
	return false;
}

bool FDbConnectionHandle::RollbackTransaction()
{
	SQLITE_TRY
		if (IsValidTransaction())
		{
			TransactionPtr.Reset();
			return true;
		}
	SQLITE_CATCH(Rollback)

	return false;
}

bool FDbConnectionHandle::Execute(const FString& Query)
{
	SQLITE_TRY
		if (IsValid())
		{
			auto cQuery = std::string(TCHAR_TO_UTF8(*Query));
			ConnectionPtr->exec(cQuery);
			return true;
		}
	SQLITE_CATCH(Db Execute)

	return false;
}

FSqliteColumn FDbConnectionHandle::Fetch(const FString& Query)
{
	SQLITE_TRY
		if (IsValid())
		{
			auto cQuery = std::string(TCHAR_TO_UTF8(*Query));
			return ConnectionPtr->execAndGet(cQuery);
		}
	SQLITE_CATCH(Fetch)

	return {};
}

FDbStatement FDbConnectionHandle::Query(const FString& Query)
{
	SQLITE_TRY
		if (IsValid())
		{
			return FDbStatement{*this, Query};
		}
	SQLITE_CATCH(Query)

	return {};
}

FDbStatement::FDbStatement(const FDbConnectionHandle& Handle, const FString& Query)
{
	SQLITE_TRY
		if (Handle.IsValid())
		{
			auto cQuery = std::string(TCHAR_TO_UTF8(*Query));
			StatementPtr = MakeShared<SQLite::Statement>(*Handle.ConnectionPtr, cQuery);
		}
	SQLITE_CATCH(Create Statement)
}

int32 FDbStatement::Execute()
{
	SQLITE_TRY
		if (IsValid())
		{
			return StatementPtr->exec();
		}
	SQLITE_CATCH(Statement Execute)

	return -1;
}

bool FDbStatement::Fetch()
{
	SQLITE_TRY
		if (IsValid())
		{
			return StatementPtr->executeStep();
		}
	SQLITE_CATCH(Statement Fetch)

	return false;
}

void FDbStatement::ClearBinds()
{
	SQLITE_TRY
		StatementPtr->clearBindings();
	SQLITE_CATCH(Clear Binds)
}

void FDbStatement::Reset()
{
	SQLITE_TRY
		StatementPtr->reset();
	SQLITE_CATCH(Reset Statement)
}

TOptional<SQLite::Column> FDbStatement::Get(const FString& Column)
{
	SQLITE_TRY
		if (!Column.IsEmpty())
		{
			return StatementPtr->getColumn(TCHAR_TO_UTF8(*Column));
		}
	SQLITE_CATCH(Fetch Column)

	return {};
}

TOptional<SQLite::Column> FDbStatement::Get(int32 Index)
{
	SQLITE_TRY
		if (Index >= 0)
		{
			return StatementPtr->getColumn(Index);
		}
	SQLITE_CATCH(Fetch Column)

	return {};
}

SQLite::Statement* FDbStatement::Raw() const
{
	return StatementPtr.Get();
}
