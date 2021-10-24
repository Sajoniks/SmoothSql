// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "SQLiteCpp/Column.h"
#include "ExecuteQueryAsync.generated.h"

class USqliteStatement;
class UDatabaseSingleton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQueryResultOutputPin, USqliteStatement*, Statement);	
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FQueryCompleteOutputPin);




/**
 * Action to call 
 */
UCLASS()
class UExecuteQueryAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	void Step();
	
protected:

	static bool bActive;			///< If any async query is active
	UObject* WorldContext;			///< WorldContext
	USqliteStatement* Statement;	///< Ptr to current statement object
	UDatabaseSingleton* Singleton;	///< Ptr to database singleton
	

	FTimerHandle TimerHandle;		///< Query execute timer

public:

	UPROPERTY(BlueprintAssignable)
	FQueryResultOutputPin Body;
	
	UPROPERTY(BlueprintAssignable)
	FQueryCompleteOutputPin Completed;
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "SmoothSqlite|Query")
	static UExecuteQueryAsync* ForEachResultAsync(UObject* WorldContextObject, USqliteStatement* Statement);

	virtual void Activate() override;
	virtual void BeginDestroy() override;

private:

	TQueue<TUniquePtr<SQLite::Column>> Results;
};
