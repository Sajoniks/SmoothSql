// Fill out your copyright notice in the Description page of Project Settings.


#include "SqliteTransaction.h"

#include "SmoothSql.h"

bool USqliteTransaction::InitTransaction(SQLite::Database& db)
{
	try
	{
		Transaction.Reset(new SQLite::Transaction(db));
		return true;
	}
	catch (SQLite::Exception& e)
	{
		UE_LOG(LogSmoothSqlite, Error, L"Failed to init transaction: %s", *FString(e.getErrorStr()))
	}

	return false;
}

void USqliteTransaction::Rollback()
{
	if (Transaction)
	{
		Transaction.Reset();
		MarkPendingKill();
	}
}

void USqliteTransaction::Commit()
{
	if (Transaction)
	{
		Transaction->commit();
		Transaction.Reset();
		MarkPendingKill();
	}
}

USqliteTransaction::~USqliteTransaction()
{
	Rollback();
}
