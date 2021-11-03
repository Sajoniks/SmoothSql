// Fill out your copyright notice in the Description page of Project Settings.


#include "SqliteStatement.h"

#include "SmoothSql.h"
#include "SqliteDatabase.h"

bool USqliteStatement::InitQuery(USqliteDatabase* Connection, const FString& Query)
{
	if (!Query.IsEmpty())
	{
		try
		{
			// Convert
			std::string QueryStr = TCHAR_TO_UTF8(*Query);
			// Get Connection
			SQLite::Database& Con = *Connection->GetDatabaseConnection();
				
			Statement.Reset(new SQLite::Statement(Con, QueryStr));
		
			return true;
		}
		catch (SQLite::Exception& e)
		{
			UE_LOG(LogSmoothSqlite, Error, L"%s", *FString(e.getErrorStr()))
		}
	}

	MarkPendingKill();
	return false;
}

void USqliteStatement::CloseStatement()
{
	if (Statement)
	{
		Statement.Release();
		MarkPendingKill();
	}
}

void USqliteStatement::Reset()
{
	if (Statement)
	{
		try
		{
			Statement->reset();
		}
		catch (SQLite::Exception& e)
		{
			UE_LOG(LogSmoothSqlite, Error, L"Error occcured while Reset: %s", *FString(e.getErrorStr()))
		}
	}
}

void USqliteStatement::ClearBindings()
{
	if (Statement)
	{
		try
		{
			Statement->clearBindings();
		}
		catch (SQLite::Exception& e)
		{
			UE_LOG(LogSmoothSqlite, Error, L"Error occured while ClearBindings: %s", *FString(e.getErrorStr()))
		}
	}
}

void USqliteStatement::ResetAll()
{
	Reset();
	ClearBindings();
}

bool USqliteStatement::Execute()
{
	try
	{
		if (Statement)
		{
			Statement->exec();
			return true;
		}
	}
	catch (SQLite::Exception& e)
	{
		UE_LOG(LogSmoothSqlite, Error, L"%s", *FString(e.getErrorStr()))
	}

	return false;
}

bool USqliteStatement::Fetch()
{
	try
	{
		if (Statement)
		{
			return Statement->executeStep();
		}
	}
	catch (SQLite::Exception& e)
	{
		UE_LOG(LogSmoothSqlite, Error, L"%s", *FString(e.getErrorStr()))
	}
	
	return false;
}

bool USqliteStatement::IsDone() const
{
	return Statement ? Statement->isDone() : false;
}


SQLite::Statement* USqliteStatement::GetStatement() const
{
	return Statement.Get();
}

USqliteStatement::~USqliteStatement()
{
	CloseStatement();
}

