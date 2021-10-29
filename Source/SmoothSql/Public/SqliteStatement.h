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
UCLASS(BlueprintType, DefaultToInstanced, EditInlineNew)
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
	 * Close this statement. Statement is not valid after this call
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Statement")
	void CloseStatement();

	virtual void BeginDestroy() override;

	/**
	 *	Reset this statement to make it ready for a new execution
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Statement")
	void Reset();

	/**
	 *	Clear away all bindings of this prepared statement
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Statement")
	void ClearBindings();
	
	/**
	 * Execute one-step query without result set
	 * @return True, if successful execution
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Statement")
	bool Execute();

	/**
	 * Execute query step 
	 * @return True, if row is ready. False, if exhausted
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Statement")
	bool Fetch();
	
	/**
	 * Check if result set is exhausted
	 * @return true, if can fetch more
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Statement")
	bool IsDone() const;

	SQLite::Statement* GetStatement() const;

private:
	
	TUniquePtr<SQLite::Statement> Statement;	///< Wrapped SQL Statement
	TWeakObjectPtr<USqliteDatabase> OwningConnection;
};
