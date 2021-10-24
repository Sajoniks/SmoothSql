// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/SmoothSqliteDataTypes.h"

#pragma push_macro("check")
#undef check

#include "SQLiteCpp/Database.h"

#pragma pop_macro("check")


#include "SqliteDatabase.generated.h"


UENUM(BlueprintType)
enum class ESqlStatementResult : uint8
{
	Then,
	Done,
	Error
};


class UDatabaseSingleton;

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
	 * Execute SQL statement (Async)
	 */
	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = "StatementResult", Latent, WorldContext="WorldContextObject", LatentInfo = "LatentInfo"), Category="SmoothSqlite|Query")
	void ExecuteAsync(UObject* WorldContextObject, struct FLatentActionInfo LatentInfo, ESqlStatementResult& StatementResult);

	
	virtual ~USqliteDatabase() override;


private:
	
	TUniquePtr<SQLite::Database> Database; ///< Underlaying database
	UDatabaseSingleton* Singleton;		   ///< Database manager
};
