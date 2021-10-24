// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/SmoothSqliteDataTypes.h"
#include "SQLiteCpp/Database.h"


#include "SqliteDatabase.generated.h"



class UDatabaseSingleton;
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
	 * Get wrapped sqlite database connection
	 * @return Ptr to SQLite::Database
	 */
	SQLite::Database* GetDatabaseConnection() const;

	/**
	 * Create query (returns values)
	 * @param [in] QueryString SQL query string
	 * @return Statement object
	 */
	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Query")
	USqliteStatement* CreateQuery(const FString& QueryString);
	
	
	virtual ~USqliteDatabase() override;


private:
	
	TUniquePtr<SQLite::Database> Database; ///< Underlaying database
	UDatabaseSingleton* Singleton;		   ///< Database manager
};
