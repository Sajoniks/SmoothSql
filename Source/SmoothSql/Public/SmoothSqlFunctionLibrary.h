// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/SmoothSqliteDataTypes.h"
#include "SmoothSqlFunctionLibrary.generated.h"


class USqliteDatabase;
class USqliteStatement;

#define DB_FUNC_SIGNATURE(ReturnType, Func) \
UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Connection") \
static ReturnType Func(UPARAM(ref) FDbConnectionHandle& Handle

#define CREATE_DB_FUNCTION(ReturnType, Func) \
DB_FUNC_SIGNATURE(ReturnType, Func))

#define CREATE_DB_FUNCTION_OneParam(ReturnType, Func, Param1Type, Param1Name) \
DB_FUNC_SIGNATURE(ReturnType, Func), Param1Type Param1Name)



UCLASS()
class SMOOTHSQL_API USmoothSqlFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_Int(UPARAM(ref) FDbStatement& Target, const FString& Param, int32 Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_Int64(UPARAM(ref) FDbStatement& Target, const FString& Param, int64 Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_Float(UPARAM(ref) FDbStatement& Target, const FString& Param, float Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_String(UPARAM(ref) FDbStatement& Target, const FString& Param, const FString& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_Text(UPARAM(ref) FDbStatement& Target, const FString& Param, const FText& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_Name(UPARAM(ref) FDbStatement& Target, const FString& Param, const FName& Value);


	/// Statement Getters
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query") 
	static int32 GetInt(UPARAM(ref) FDbStatement& Target, const FString& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query") 
	static int64 GetInt64(UPARAM(ref) FDbStatement& Target, const FString& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query") 
	static float GetFloat(UPARAM(ref) FDbStatement& Target, const FString& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query") 
	static FString GetString(UPARAM(ref) FDbStatement& Target, const FString& Column);


	
	/// Column getters
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Int")) 
	static int32 GetInt_Column(UPARAM(ref) FSqliteColumn& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Int64"))  
	static int64 GetInt64_Column(UPARAM(ref) FSqliteColumn& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Float"))  
	static float GetFloat_Column(UPARAM(ref)FSqliteColumn& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get String")) 
	static FString GetString_Column(UPARAM(ref) FSqliteColumn& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Is Valid"))
	static bool IsValid_Column(UPARAM(ref) FSqliteColumn& Column);


	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static FDbStatement& K2_StepStatement(UPARAM(ref) FDbStatement& Target, bool& Success);



	
	/// Db handle related functions

	/**
	 * Open connection
	 * @return Handle to connection
	 */
	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Connection")
	static FDbConnectionHandle OpenDbConnection(const FSqliteDBConnectionParms& Parms);

	/**
	 * Make prepared statement
	 * @param [in] Handle Db connection handle
	 * @param [in] Query Query to execute
	 * @return Statement
	 */
	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Connection")
	static FDbStatement Query(UPARAM(ref) FDbConnectionHandle& Handle, const FString& Query);

	/**
	 * Check if handle is actually pointing to something
	 * @param [in] Handle Db connection handle
	 * @return True, if has valid connection
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Connection", meta = (DisplayName="Is Valid"))
	static bool IsValid_DbHandle(const FDbConnectionHandle& Handle);

	/**
	* Check if handle is actually pointing to something
	* @param [in] Handle Db connection handle
	* @return True, if has valid connection
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Connection", meta = (DisplayName="Is Valid"))
	static bool IsValid_DbTransaction(const FDbConnectionHandle& Handle);
	
	/**
	 * Begin db transaction
	 * @param [in] Handle Db connection handle
	 * @return True, if no errors 
	 */
	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Connection")
	static bool BeginTransaction(UPARAM(ref) FDbConnectionHandle& Handle);

	/**
	 * Commit transaction (if any)
	 * @param [in] Handle Db connection handle
	 * @return True, if no errors
	 */
	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Connection")
	static bool CommitTransaction(UPARAM(ref) FDbConnectionHandle& Handle);

	/**
	 * Rollback transaction (if any)
	 * @param [in] Handle Db connection handle
	 * @return True, if no errors
	 */
	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Connection")
	static bool RollbackTransaction(UPARAM(ref) FDbConnectionHandle& Handle);
	
	/**
	 * Execute query
	 * @param [in] Handle Db connection handle
	 * @param [in] Query Query to execute
	 * @return True, if no errors
	 */	
	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Connection")
	static bool Execute(UPARAM(ref) FDbConnectionHandle& Handle, const FString& Query);

	/**
	 * Execute query and get first column
	 * @param [in] Handle Db connection handle
	 * @param [in] Query Query to execute
	 * @return True, if no errors
	 */
	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Connection")
	static FSqliteColumn Fetch(UPARAM(ref) FDbConnectionHandle& Handle, const FString& Query);



	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Statement", meta = (DisplayName="Is Valid"))
	static bool IsValid_DbStatement(const FDbStatement& Handle);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Statement", meta = (DisplayName="Is Valid"))
	static bool IsDone(const FDbStatement& Handle);

	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Statement", meta=(DisplayName="Execute"))
	static int32 Execute_Statement(UPARAM(ref) FDbStatement& Statement);

	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Statement", meta=(DisplayName="Fetch"))
	static bool Fetch_Statement(UPARAM(ref) FDbStatement& Statement);

	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Statement")
	static void ClearBinds(UPARAM(ref) FDbStatement& Statement);

	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Statement")
	static void Reset(UPARAM(ref) FDbStatement& Statement);	


	
	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Statement")
	static void CloseStatement(UPARAM(ref) FDbStatement& Statement);
	
	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Connection")
    static void CloseConnection(UPARAM(ref) FDbConnectionHandle& Handle);
	
};
