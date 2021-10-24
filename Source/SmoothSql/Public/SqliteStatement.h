// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SQLiteCpp/Backup.h"
#include "UObject/NoExportTypes.h"
#include "SqliteStatement.generated.h"

class USqliteDatabase;

/**
 * Wrapper over SQLite::Statement
 */
UCLASS(BlueprintType)
class SMOOTHSQL_API USqliteStatement : public UObject
{
	GENERATED_BODY()

public:

	/**
	 * Init object with query
	 * @param [in] Query Query string
	 * @return True, if init succesfully
	 */
	bool InitQuery(USqliteDatabase* Connection, const FString& Query);

	/**
	 * Execute one-step query without result set
	 * @return True, if successful execution
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Statement", meta=(DisplayName="Execute (No Result)"))
	bool Execute_OneStep();

	/**
	 * Execute query step 
	 * @return True, if successful execution
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Statement", meta=(DisplayName="StepQuery"))
	bool Step();
	
	/**
	 * Check if result set is exhausted
	 * @return true, if can fetch more
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Statement")
	bool IsDone() const;

	SQLite::Statement* GetStatement() const;

	virtual ~USqliteStatement() override;

private:

	TUniquePtr<SQLite::Statement> Statement;	///< Wrapped SQL Statement
};
