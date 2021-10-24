// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SmoothSqliteDataTypes.generated.h"





USTRUCT(BlueprintType)
struct FSqliteDBConnectionParms
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DBConnectionParams")
	FString DBName = "DB";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DBConnectionParams")
	FString Folder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DBConnectionParams")
	int32 BusyTimeout = 0;
};