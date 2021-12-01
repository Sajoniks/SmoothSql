// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "BlueprintAsyncTask.generated.h"

DECLARE_DYNAMIC_DELEGATE(FAsyncTaskDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAsyncTaskPinDelegate);
/**
 * 
 */
UCLASS()
class SMOOTHSQL_API UBlueprintAsyncTask : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
	void CheckCompletion();

	UObject* WorldContext;
	FCriticalSection Mutex;

	static FTimerHandle TimerHandle;
	static bool bFinished;

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "SmoothSql|Helpers")
	static UBlueprintAsyncTask* RunAsyncTask(UObject* WorldContextObject, const FAsyncTaskDelegate& TaskToExecute);

	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable)
	FAsyncTaskPinDelegate Completed;

	virtual void BeginDestroy() override;

private:
	FAsyncTaskDelegate TaskToRun;

};
