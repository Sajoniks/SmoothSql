// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/SmoothSqliteDataTypes.h"
#include "Subsystems/EngineSubsystem.h"
#include "DatabaseSingleton.generated.h"

class USqliteDatabase;
class USqliteStatement;
class USqliteTransaction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSqliteStatementDelegate, USqliteStatement*, Statement);






UCLASS()
class UDatabaseSingleton : public UEngineSubsystem
{
	GENERATED_BODY()
	
public:

	/**
	 * Obtain connection to the database with given set of parameters
	 * @param [in] Parms Connection parameters
	 * @return Database connection object
	 * @warning Can be null
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Connection")
	USqliteDatabase* CreateConnection(const FSqliteDBConnectionParms& Parms) const;
	
	/**
	 * Explicitly destroyes the underlaying connection
	 * @param [in] DB Connection to close
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Connection")
	void CloseConnection(USqliteDatabase* DB);

	
	virtual void Deinitialize() override;

	
	UPROPERTY(BlueprintAssignable)
	FSqliteStatementDelegate OnAsyncQueryBegin;

	UPROPERTY(BlueprintAssignable)
	FSqliteStatementDelegate OnAsyncQueryEnd;


	
private:
	
	mutable TMap<FObjectKey, USqliteDatabase*> Connections;	///< Allocated connections
};
