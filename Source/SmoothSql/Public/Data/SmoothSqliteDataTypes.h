// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SmoothSql.h"
#include "SQLiteCpp/Column.h"
#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Transaction.h"
#include "SmoothSqliteDataTypes.generated.h"


USTRUCT(BlueprintType)
struct FSqliteDBConnectionParms
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DBConnectionParams")
	FString DBName = "DB";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DBConnectionParams")
	FString Folder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DBConnectionParams")
	int32 BusyTimeout = 0;
};




/// Wrapper over SQLite Column
USTRUCT(BlueprintType)
struct FSqliteColumn
{
	GENERATED_BODY()

	FSqliteColumn()
		: ColumnPtr(nullptr)
	{
		
	}

	FSqliteColumn(const SQLite::Column& Column)
		: ColumnPtr(new SQLite::Column(Column))
	{
		
	}

	FSqliteColumn(const FSqliteColumn& Column)
		: ColumnPtr(new SQLite::Column( *Column.ColumnPtr ))
	{
		
	}

	FSqliteColumn& operator=(const FSqliteColumn& Column)
	{
		if (&Column != this && Column.ColumnPtr.IsValid())
		{
			ColumnPtr.Reset(new SQLite::Column(*Column.ColumnPtr));
		}

		return *this;
	}
	
	TUniquePtr<SQLite::Column> ColumnPtr;
};

template<>
struct TStructOpsTypeTraits<FSqliteColumn> : TStructOpsTypeTraitsBase2<FSqliteColumn>
{
	enum
	{
		WithCopy = true
	};
};


USTRUCT(BlueprintType)
struct FDbConnectionHandle
{
	GENERATED_BODY()

	friend struct FDbStatement;
	
	FDbConnectionHandle()
		: ConnectionPtr(nullptr)
	{}
	
	FDbConnectionHandle(const FSqliteDBConnectionParms& Params);

	
	bool BeginTransaction();
	bool CommitTransaction();
	bool RollbackTransaction();

	bool Execute(const FString& Query);
	FSqliteColumn Fetch(const FString& Query);

	struct FDbStatement Query(const FString& Query);

	bool IsValid() const {return ConnectionPtr.IsValid();}
	bool IsValidTransaction() const {return TransactionPtr.IsValid();}

	void Invalidate()
	{
		ConnectionPtr.Reset();
		TransactionPtr.Reset();
	}

	UPROPERTY(BlueprintReadOnly, Category="DbHandle")
	FSqliteDBConnectionParms ConnectionParms;
	
private:
	TSharedPtr<SQLite::Database> ConnectionPtr;
	TSharedPtr<SQLite::Transaction> TransactionPtr;
};


USTRUCT(BlueprintType)
struct FDbStatement
{
	GENERATED_BODY()

	FDbStatement()
		: StatementPtr(nullptr) {}

	FDbStatement(const FDbConnectionHandle& Handle, const FString& Query);
	
	int32 Execute();
	bool Fetch();

	void ClearBinds();
	void Reset();

	template<class T>
	void Bind(const FString& ParamName, const T& Value);
	
	TOptional<SQLite::Column> Get(const FString& Column);
	TOptional<SQLite::Column> Get(int32 Index);

	SQLite::Statement* Raw() const;

	bool IsValid() const {return StatementPtr.IsValid(); }
	bool IsDone() const {return IsValid() ? StatementPtr->isDone() : true;}

	void Invalidate() {StatementPtr.Reset();}

private:
	TSharedPtr<SQLite::Statement> StatementPtr;
};


template <class T>
void FDbStatement::Bind(const FString& ParamName, const T& Value)
{
	if (!ParamName.IsEmpty() && IsValid())
	{
		try
		{
			StatementPtr->bind(TCHAR_TO_UTF8(*ParamName), Value);
		}
		catch (SQLite::Exception& e)
		{
			(void)e;
			UE_LOG(LogSmoothSqlite, Error, L"Failed to bind value to param %s", *ParamName);
		}
	}
}

