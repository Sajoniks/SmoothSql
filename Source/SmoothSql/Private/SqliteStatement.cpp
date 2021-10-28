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
		UE_LOG(LogSmoothSqlite, Display, L"Received statement close")
		
		Statement.Release();
		MarkPendingKill();
	}
}

void USqliteStatement::Reset()
{
	if (Statement)
	{
		Statement->reset();
	}
}

void USqliteStatement::ClearBindings()
{
	if (Statement)
	{
		Statement->clearBindings();
	}
}

bool USqliteStatement::Execute_OneStep()
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

bool USqliteStatement::Step()
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

