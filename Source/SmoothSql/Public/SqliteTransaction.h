// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SQLiteCpp/Transaction.h"
#include "UObject/NoExportTypes.h"
#include "SqliteTransaction.generated.h"

/**
 * 
 */
UCLASS()
class SMOOTHSQL_API USqliteTransaction : public UObject
{
	GENERATED_BODY()


public:

	bool InitTransaction(SQLite::Database& db);

	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Transaction")
	void Rollback();

	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Transaction")
	void Commit();

	virtual ~USqliteTransaction() override;
	
private:

	TUniquePtr<SQLite::Transaction> Transaction;
};
