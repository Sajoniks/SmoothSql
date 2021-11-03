// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/SmoothSqliteDataTypes.h"
#include "SmoothSqlFunctionLibrary.generated.h"


class USqliteDatabase;
class USqliteStatement;

UCLASS()
class SMOOTHSQL_API USmoothSqlFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_Int(USqliteStatement* Target, const FString& Param, int32 Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_Int64(USqliteStatement* Target, const FString& Param, int64 Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_Float(USqliteStatement* Target, const FString& Param, float Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_String(USqliteStatement* Target, const FString& Param, const FString& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_Text(USqliteStatement* Target, const FString& Param, const FText& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_Name(USqliteStatement* Target, const FString& Param, const FName& Value);


	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query") 
	static int32 GetInt(USqliteStatement* Target, const FString& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query") 
	static int64 GetInt64(USqliteStatement* Target, const FString& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query") 
	static float GetFloat(USqliteStatement* Target, const FString& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query") 
	static FString GetString(USqliteStatement* Target, const FString& Column);


	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Int")) 
	static int32 GetInt_Column(UPARAM(ref) FSqliteColumn& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Int64"))  
	static int64 GetInt64_Column(UPARAM(ref) FSqliteColumn& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Float"))  
	static float GetFloat_Column(UPARAM(ref)FSqliteColumn& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get String")) 
	static FString GetString_Column(UPARAM(ref) FSqliteColumn& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query")
	static bool IsValid_Column(UPARAM(ref) FSqliteColumn& Column);


	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static USqliteStatement* K2_StepStatement(USqliteStatement* Target, bool& Success);



	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Connection")
	static USqliteDatabase* GetDatabase(const FSqliteDBConnectionParms& Parms);
};
