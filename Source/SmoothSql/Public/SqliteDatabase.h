// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/SmoothSqliteDataTypes.h"
#include "SQLiteCpp/Database.h"


#include "SqliteDatabase.generated.h"



class UDatabaseSingleton;
class USqliteTransaction;
class USqliteStatement;

UCLASS(BlueprintType)
class SMOOTHSQL_API USqliteDatabase : public UObject
{
	GENERATED_BODY()

public:
	// Default constructor
	USqliteDatabase(const FObjectInitializer& ObjectInitializer);

	/**
	 * Try to setup DB connection
	 * @param [in] Params DB connection params
	 * @return True, if connection was established
	 */
	bool InitConnection(const FSqliteDBConnectionParms& Params);

	/**
	 * Explicitly close the connection
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Connection")
	void CloseConnection();

	/**
	 * Check if underlaying object has any valid DB connection
	 * @return True, if valid DB connection object is associated with this file
	 */
	bool HasValidConnection() const;

	/**
	 * Commit transaction (if any)
	 * @return True, if commited
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Connection")
	bool CommitTransaction();

	/**
	 *	Rollback transaction (if any)
	 *	@return True, if success
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Connection")
	bool RollbackTransaction();

	
	/**
	 * Get wrapped sqlite database connection
	 * @return Ptr to SQLite::Database
	 */
	SQLite::Database* GetDatabaseConnection() const;

	/**
	 * Create query
	 * @param [in] QueryString SQL query string
	 * @return Statement object
	 */
	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Query")
	USqliteStatement* CreateQuery(const FString& QueryString);

	/**
	 * Start SQL transaction
	 */
	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Query")
	USqliteTransaction* BeginTransaction();

	/**
	 * If any transaction is active
	 */
	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Query")
	bool HasActiveTransaction() const;

	/**
	 * Get current active transaction (if any)
	 * @return Transaction object
	 */
	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Query")
	USqliteTransaction* GetTransaction() const;
		
	
	/**
	 * Execute query without result set
	 * @param [in] Query Query to execute
	 * @return True if successful
	 * @note Multiple statements can be separated with ";"
	 */
	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Query", meta=(DisplayName="Execute (No Result)"))
	bool Execute_OneStep(const FString& Query);
		
	
	virtual ~USqliteDatabase() override;


private:
	
	TUniquePtr<SQLite::Database> Database;	///< Underlaying database

	UPROPERTY()
	UDatabaseSingleton* Singleton;			///< Database manager

	UPROPERTY()
	USqliteTransaction* CurrentTransaction; ///< Current transaction
};
