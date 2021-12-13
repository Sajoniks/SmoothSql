// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/SmoothSqliteDataTypes.h"
#include "SmoothSqlFunctionLibrary.generated.h"


class UDbObject;
class UDbStmt;


UCLASS()
class SMOOTHSQL_API USmoothSqlFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_Int(UDbStmt* Target, const FString& Param, int32 Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_Int64(UDbStmt* Target, const FString& Param, int64 Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_Float(UDbStmt* Target, const FString& Param, float Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_String(UDbStmt*  Target, const FString& Param, const FString& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_Text(UDbStmt*  Target, const FString& Param, const FText& Value);

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static void K2_BindQueryParam_Name(UDbStmt*  Target, const FString& Param, const FName& Value);


	/// Statement Getters
	
#define DB_STMT_GETTER_IDX(Name, Type)\
	static Type Get##Name##_Stmt_Idx(UDbStmt* Target, int32 ColumnIdx);
#define DB_STMT_GETTER_NAME(Name, Type)\
	static Type Get##Name##_Stmt_Str(UDbStmt* Target, const FString& ColumnName);

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Int (Using Index)")) 
	static int32 GetInt_Stmt_Idx(UDbStmt* Target, int32 ColumnIdx);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Int (Using Name)")) 
	static int32 GetInt_Stmt_Str(UDbStmt* Target, const FString& ColumnName);
	
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Int64 (Using Index)")) 
	static int64 GetInt64_Stmt_Idx(UDbStmt* Target, int32 ColumnIdx);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Int64 (Using Name)")) 
	static int64 GetInt64_Stmt_Str(UDbStmt* Target, const FString& ColumnName);

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Float (Using Name)")) 
	static float GetFloat_Stmt_Str(UDbStmt* Target, const FString& ColumnName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Float (Using Idx)")) 	
	static float GetFloat_Stmt_Idx(UDbStmt* Target, int32 ColumnIdx);

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get String (Using Name)"))
	static FString GetString_Stmt_Str(UDbStmt* Target, const FString& ColumnName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get String (Using Idx)")) 
	static FString GetString_Stmt_Idx(UDbStmt* Target, int32 ColumnIdx);

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Name (Using Idx)"))
	static FName GetName_Stmt_Idx(UDbStmt* Target, int32 ColumnIdx);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Name (Using Name)"))
	static FName GetName_Stmt_Str(UDbStmt* Target, const FString& ColumnName);
	
#undef DB_STMT_GETTER_IDX
#undef DB_STMT_GETTER_NAME	


#define DB_COL_GETTER(Type, Name)\
	static Type Get##Name##_Column(UPARAM(ref) FSqliteColumn& Column);
	
	/// Column getters
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Int")) 
	static int32 GetInt_Column(UPARAM(ref) FSqliteColumn& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Int64"))  
	static int64 GetInt64_Column(UPARAM(ref) FSqliteColumn& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Float"))  
	static float GetFloat_Column(UPARAM(ref)FSqliteColumn& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get String")) 
	static FString GetString_Column(UPARAM(ref) FSqliteColumn& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Get Name"))
	static FName GetName_Column(UPARAM(ref) FSqliteColumn& Column);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SmoothSqlite|Query", meta=(DisplayName="Is Valid"))
	static bool IsValid_Column(UPARAM(ref) FSqliteColumn& Column);

#undef DB_COL_GETTER
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Category="SmoothSqlite|Bind")
	static UDbStmt* K2_StepStatement(UDbStmt* Target, bool& Success);



	
	/// Db handle related functions

	/**
	 * @brief Open connection
	 *
	 * Opens connection to SQLite Database using parameters defined in DbDefaultSettings
	 * @return Handle to connection
	 */
	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Connection")
	static UDbObject* OpenDbConnection(UPARAM(meta = (Bitmask, BitmaskEnum="EDbOpenFlags")) int32 OpenFlags = 2);

	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="SmoothSqlite|Connection", meta=(DisplayName="Is Valid (DbObject)"))
	static bool IsValid_DbConnection(UDbObject* Object);

	/**
	 * @brief Open database and execute SQL
	 *
	 * Opens connection in-place, executes code and then closes connection
	 * @param [in] SQL SQL to execute
	 * @return Number of changes
	 */
	UFUNCTION(BlueprintCallable, Category = "SmoothSqlite|Connection")
	static int32 ExecuteInline(const FString& SQL);
};
