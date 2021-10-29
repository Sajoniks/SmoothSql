// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/SmoothSqliteDataTypes.h"
#include "Subsystems/EngineSubsystem.h"
#include "DatabaseSingleton.generated.h"

class USqliteDatabase;
class USqliteStatement;
class USqliteTransaction;
class UDeferredDatabaseCleanupAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSqliteStatementDelegate, USqliteStatement*, Statement);






UCLASS()
class UDatabaseSingleton : public UEngineSubsystem
{
	GENERATED_BODY()

	bool CleanupTick(float InDeltaTime, USqliteDatabase* Db);
	
public:

	/**
	 * Obtain connection to the database with given set of parameters
	 * @param [in] Parms Connection parameters
	 * @return Database connection object
	 * @warning Can be null
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Connection")
	USqliteDatabase* CreateConnection(const FSqliteDBConnectionParms& Parms, TSubclassOf<UDeferredDatabaseCleanupAction> CleanupActionClass) const;
	
	
	void FinalizeDb(USqliteDatabase* Db);

	/**
	 * Explicitly destroyes the underlaying connection
	 * @param [in] DB Connection to close
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSqlite|Connection", meta=(WorldContext="WorldContext"))
	void CloseConnection(UObject* WorldContext, USqliteDatabase* DB);

	
	virtual void Deinitialize() override;

	
	UPROPERTY(BlueprintAssignable)
	FSqliteStatementDelegate OnAsyncQueryBegin;

	UPROPERTY(BlueprintAssignable)
	FSqliteStatementDelegate OnAsyncQueryEnd;


	
private:
	
	mutable TMap<FObjectKey, USqliteDatabase*> Connections;	///< Allocated connections

	UPROPERTY()
	mutable UClass* CleanupAction;

	FDelegateHandle TickHandle;
};
