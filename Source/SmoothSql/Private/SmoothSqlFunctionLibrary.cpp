// Fill out your copyright notice in the Description page of Project Settings.


#include "SmoothSqlFunctionLibrary.h"

#include "SmoothSql.h"
#include "SqliteStatement.h"
#include "SQLiteCpp/Exception.h"





void USmoothSqlFunctionLibrary::K2_BindQueryParam_Int(USqliteStatement* Target, const FString& Param, int32 Value)
{
	if (!Target) return;

	
	auto& sqlStatement = *Target->GetStatement();
	const char* c_ParamName = TCHAR_TO_UTF8(*(":" + Param));

	 try
	 {
		sqlStatement.bind(c_ParamName, Value);
	 }
	 catch (const SQLite::Exception& e)
	 {
	 	(void)e;
	 	UE_LOG(LogSmoothSqlite, Error, L"Failed to bind value '%d' to param '%s'", Value, *Param);
	 }
}

void USmoothSqlFunctionLibrary::K2_BindQueryParam_Int64(USqliteStatement* Target, const FString& Param, int64 Value)
{
	if (!Target) return;


	auto& sqlStatement = *Target->GetStatement();
	const char* c_ParamName = TCHAR_TO_UTF8(*(":" + Param));
	
	try
	{
		sqlStatement.bind(c_ParamName, Value);
	}
	catch (const SQLite::Exception& e)
	{
		(void)e;
		UE_LOG(LogSmoothSqlite, Error, L"Failed to bind value '%d' to param '%s'", Value, *Param);
	}

}

void USmoothSqlFunctionLibrary::K2_BindQueryParam_Float(USqliteStatement* Target, const FString& Param, float Value)
{
	if (!Target) return;

	
	auto& sqlStatement = *Target->GetStatement();
	const char* c_ParamName = TCHAR_TO_UTF8(*(":" + Param));

	try
	{
		sqlStatement.bind(c_ParamName, Value);
	}
	catch (const SQLite::Exception& e)
	{
		(void)e;
		UE_LOG(LogSmoothSqlite, Error, L"Failed to bind value '%d' to param '%s'", Value, *Param);
	}
}

void USmoothSqlFunctionLibrary::K2_BindQueryParam_String(USqliteStatement* Target, const FString& Param,
                                                         const FString& Value)
{
	if (!Target) return;
	
	auto& sqlStatement = *Target->GetStatement();
	const char* c_ParamName = TCHAR_TO_UTF8(*(":" + Param));
	const char* c_Val = TCHAR_TO_UTF8(*Value);
	
	try
	{
		sqlStatement.bind(c_ParamName, c_Val);
	}
	catch (const SQLite::Exception& e)
	{
		(void)e;
		UE_LOG(LogSmoothSqlite, Error, L"Failed to bind value '%s' to param '%s'", *Value, *Param);
	}
}

void USmoothSqlFunctionLibrary::K2_BindQueryParam_Text(USqliteStatement* Target, const FString& Param,
	const FText& Value)
{
	K2_BindQueryParam_String(Target, Param, Value.ToString());
}

void USmoothSqlFunctionLibrary::K2_BindQueryParam_Name(USqliteStatement* Target, const FString& Param,
	const FName& Value)
{
	K2_BindQueryParam_String(Target, Param, Value.ToString());
}

int32 USmoothSqlFunctionLibrary::GetInt(USqliteStatement* Target, const FString& Column)
{
	if (!Target) return int32{};

	auto& sqlStatement = *Target->GetStatement();
	const char* ColName = TCHAR_TO_UTF8(*Column);

	try
	{
		return sqlStatement.getColumn(ColName).getInt();
	}
	catch (const SQLite::Exception& e)
	{
		UE_LOG(LogSmoothSqlite, Error, L"Error retrieving column: %s (%d)", *FString(e.getErrorStr()), e.getErrorCode());
		return int32{};
	}
}

int64 USmoothSqlFunctionLibrary::GetInt64(USqliteStatement* Target, const FString& Column)
{
	if (!Target) return int64{};

	auto& sqlStatement = *Target->GetStatement();
	const char* ColName = TCHAR_TO_UTF8(*Column);

	
	try
	{
		return sqlStatement.getColumn(ColName).getInt64();
	}
	catch (const SQLite::Exception& e)
	{
		UE_LOG(LogSmoothSqlite, Error, L"Error retrieving column: %s (%d)", *FString(e.getErrorStr()), e.getErrorCode());
		return int64{};
	}
}

float USmoothSqlFunctionLibrary::GetFloat(USqliteStatement* Target, const FString& Column)
{
	if (!Target) return float{};

	auto& sqlStatement = *Target->GetStatement();
	const char* ColName = TCHAR_TO_UTF8(*Column);
	
	try
	{
		return sqlStatement.getColumn(ColName).getDouble();
	}
	catch (const SQLite::Exception& e)
	{
		UE_LOG(LogSmoothSqlite, Error, L"Error retrieving column: %s (%d)", *FString(e.getErrorStr()), e.getErrorCode());
		return float{};
	}
}

FString USmoothSqlFunctionLibrary::GetString(USqliteStatement* Target, const FString& Column)
{
	if (!Target) return FString{};

	auto& sqlStatement = *Target->GetStatement();
	const char* ColName = TCHAR_TO_UTF8(*Column);
	
	try
	{
		return FString(UTF8_TO_TCHAR( sqlStatement.getColumn(ColName).getString().c_str() ));
	}
	catch (const SQLite::Exception& e)
	{
		UE_LOG(LogSmoothSqlite, Error, L"Error retrieving column: %s (%d)", *FString(e.getErrorStr()), e.getErrorCode());
		return FString{};
	}
}

USqliteStatement* USmoothSqlFunctionLibrary::K2_StepStatement(USqliteStatement* Target, bool& Success)
{
	if (Target)
	{
		Success = Target->Step();
	}

	return Target;
}

