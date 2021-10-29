// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SmoothSql.h"
#include "SQLiteCpp/Column.h"
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
/// Non-copyable 
USTRUCT(BlueprintType)
struct FSqliteColumn
{
	GENERATED_BODY()

	FSqliteColumn()
		: Column(nullptr) {}
	
	FSqliteColumn(SQLite::Column& Column)
		: Column(new SQLite::Column(Column)) {}

	TUniquePtr<SQLite::Column> Column;
};


template<>
struct TStructOpsTypeTraits<FSqliteColumn> : TStructOpsTypeTraitsBase2<FSqliteColumn>
{
	enum
	{
		WithCopy = false
	};
};