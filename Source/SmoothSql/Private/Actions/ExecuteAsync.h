// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/SmoothSqliteDataTypes.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "ExecuteAsync.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAsyncExecDelegate);

class USqliteStatement;
class USqliteDatabase;
/**
 * 
 */
UCLASS()
class SMOOTHSQL_API UExecuteAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	UObject* InWorldContextObject;
	FDbConnectionHandle InDb;
	FDbStatement InStatement;
	FString InQuery;

	FTimerHandle CompletionCheckHandle;
	void CheckCompletion();

public:

	UPROPERTY(BlueprintAssignable)
	FAsyncExecDelegate Completed;
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "SmoothSqlite|Query")
	static UExecuteAsync* ExecuteAsync_Database(UObject* WorldContextObject, const FDbConnectionHandle& Database, const FString& Query);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "SmoothSqlite|Query")
	static UExecuteAsync* CommitTransactionAsync(UObject* WorldContextObject, const FDbConnectionHandle& Database);
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "SmoothSqlite|Query")
	static UExecuteAsync* ExecuteAsync_Statement(UObject* WorldContextObject, const FDbStatement& Statement);

	virtual void Activate() override;
	virtual void BeginDestroy() override;
};
