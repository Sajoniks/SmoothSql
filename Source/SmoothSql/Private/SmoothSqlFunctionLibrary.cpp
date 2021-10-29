// Fill out your copyright notice in the Description page of Project Settings.


#include "SmoothSqlFunctionLibrary.h"

#include "SmoothSql.h"
#include "SqliteStatement.h"
#include "Data/SmoothSqliteDataTypes.h"
#include "SQLiteCpp/Exception.h"


void USmoothSqlFunctionLibrary::K2_BindQueryParam_Int(USqliteStatement* Target, const FString& Param, int32 Value)
{
	if (!Target) return;

	
	auto& sqlStatement = *Target->GetStatement();
	std::string c_ParamName = ":" + std::string(TCHAR_TO_UTF8(*Param));

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
	std::string c_ParamName = ":" + std::string(TCHAR_TO_UTF8(*Param));
	
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
	std::string c_ParamName = ":" + std::string(TCHAR_TO_UTF8(*Param));

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
	std::string c_ParamName = ":" + std::string(TCHAR_TO_UTF8(*Param));
	std::string c_Val = std::string(TCHAR_TO_UTF8(*Value));
	
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
	std::string ColName = TCHAR_TO_UTF8(*Column);

	try
	{
		return sqlStatement.getColumn(ColName.c_str()).getInt();
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
	std::string ColName = TCHAR_TO_UTF8(*Column);

	
	try
	{
		return sqlStatement.getColumn(ColName.c_str()).getInt64();
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
	std::string ColName = TCHAR_TO_UTF8(*Column);
	
	try
	{
		return sqlStatement.getColumn(ColName.c_str()).getDouble();
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
	std::string ColName = TCHAR_TO_UTF8(*Column);
	
	try
	{
		return FString(UTF8_TO_TCHAR( sqlStatement.getColumn(ColName.c_str()).getString().c_str() ));
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
		Success = Target->Fetch();
	}

	return Target;
}



/**
 * Simple helper template class for getting right value of the column without code duplication
 */
template<class T>
struct FColumnGetHelper
{
	static T Get(const FSqliteColumn& Row) {return T{};}
};

template<>
struct FColumnGetHelper<int32>
{
	static auto Get(const FSqliteColumn& Row)
	{
		return Row.Column->getInt();
	}
};

template<>
struct FColumnGetHelper<int64>
{
	static auto Get(const FSqliteColumn& Row)
	{
		return Row.Column->getInt64();
	}
};

template<>
struct FColumnGetHelper<FString>
{
	static auto Get(const FSqliteColumn& Row)
	{
		return FString( UTF8_TO_TCHAR(Row.Column->getString().c_str()));
	}
};

template<>
struct FColumnGetHelper<float>
{
	static auto Get(const FSqliteColumn& Row)
	{
		return (float) Row.Column->getDouble();
	}
};

// Main column getter method
template<class T>
decltype(auto) GetSomething_Column(FSqliteColumn& Row)
{
	try
	{
		return FColumnGetHelper<T>::Get(Row);
	}
	catch (SQLite::Exception& e)
	{
		UE_LOG(LogSmoothSqlite, Display, L"Error occured getting value: %s", *FString(e.getErrorStr()))
	}

	return T{};
}

int32 USmoothSqlFunctionLibrary::GetInt_Column(FSqliteColumn& Column)
{
	return GetSomething_Column<int32>(Column);
}

int64 USmoothSqlFunctionLibrary::GetInt64_Column(FSqliteColumn& Column)
{
	return GetSomething_Column<int64>(Column);
}

float USmoothSqlFunctionLibrary::GetFloat_Column(FSqliteColumn& Column)
{
	return GetSomething_Column<float>(Column);
}

FString USmoothSqlFunctionLibrary::GetString_Column(FSqliteColumn& Column)
{
	return GetSomething_Column<FString>(Column);
}
