// Fill out your copyright notice in the Description page of Project Settings.


#include "SmoothSqlFunctionLibrary.h"

#include "SmoothSql.h"
#include "SqliteDatabase.h"
#include "SqliteStatement.h"
#include "SQLiteCpp/Exception.h"


/**
 * Simple helper template for getting right value of the column without code duplication
 */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace details
{
	/// Specialize method for correct Get value from SQLite column
	template<class T>
	decltype(auto) GetFromColumn(const SQLite::Column& Col)
	{
		return T{};
	}

	/// Defined templates
	template<>
	decltype(auto) GetFromColumn<int32>(const SQLite::Column& Col)
	{
		return Col.getInt();
	}

	template<>
	decltype(auto) GetFromColumn<int64>(const SQLite::Column& Col)
	{
		return Col.getInt64();
	}

	template<>
	decltype(auto) GetFromColumn<FString>(const SQLite::Column& Col)
	{
		return FString( UTF8_TO_TCHAR(Col.getString().c_str()));
	}

	template<>
	decltype(auto) GetFromColumn<float>(const SQLite::Column& Col)
	{
		return (float) Col.getDouble();
	}
	///
}
/// Get value from FSqliteColumn
template<class T>
decltype(auto) GetFromStructColumn(FSqliteColumn& Row)
{
	if (Row.Column)
	{
		try
		{
			return details::GetFromColumn<T>(*Row.Column.Get());
		}
		catch (SQLite::Exception& e)
		{
			UE_LOG(LogSmoothSqlite, Display, L"Error occured getting value: %s", *FString(e.getErrorStr()))
		}
	}

	return T{};
}

/// Get column value from Sqlite Statement
template<class T>
static T GetFromStatement(USqliteStatement* Statement, const FString& Column)
{
	try
	{
		if (!Statement || !Statement->GetStatement()) return T{};
		return details::GetFromColumn<T>(Statement->GetStatement()->getColumn(TCHAR_TO_UTF8(*Column)));
	}
	catch (SQLite::Exception& e)
	{
		UE_LOG(LogSmoothSqlite, Error, L"Error retrieving column: %s (%d)", *FString(e.getErrorStr()), e.getErrorCode());
		return T{};
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace details
{
	/// Specialize method for correct binding value to query param
	template<class T>
	void BindQueryParam(const std::string& Param, const T& Value, SQLite::Statement& Statement)
	{
		Statement.bind(Param, Value);
	}
	
	/// Defined templates
	template<>
	void BindQueryParam(const std::string& Param, const FString& Value, SQLite::Statement& Statement)
	{
		Statement.bind(Param, std::string(TCHAR_TO_UTF8(*Value)));
	}
	///


	template<class T>
	void Log(SQLite::Exception& e, const T& Value, const FString& Param)
	{
		UE_LOG(LogSmoothSqlite, Error, L"Failed to bind value '%d' to param '%s'", Value, *Param);
	}


	template<>
	void Log<FString>(SQLite::Exception& e, const FString& Value, const FString& Param)
	{
		UE_LOG(LogSmoothSqlite, Error, L"Failed to bind value '%s' to param '%s'", *Value, *Param);
	}

	template<>
	void Log<float>(SQLite::Exception& e, const float& Value, const FString& Param)
	{
		UE_LOG(LogSmoothSqlite, Error, L"Failed to bind value '%f' to param '%s'", Value, *Param);
	}
}

/// Main helper method binding query param
template <class T>
void Bind(const FString& Param, const T& Value, USqliteStatement* Statement)
{
	try
	{
		if (!Statement || !Statement->GetStatement()) return;

		const std::string ParamName = std::string(TCHAR_TO_UTF8(*Param));
			
		details::BindQueryParam(ParamName, Value, *Statement->GetStatement());
	}
	catch (SQLite::Exception& e)
	{
		(void)e;
		details::Log(e, Value, Param);
	}
}

void USmoothSqlFunctionLibrary::K2_BindQueryParam_Int(USqliteStatement* Target, const FString& Param, int32 Value)
{
	Bind<int32>(Param, Value, Target);
}

void USmoothSqlFunctionLibrary::K2_BindQueryParam_Int64(USqliteStatement* Target, const FString& Param, int64 Value)
{
	Bind<int64>(Param, Value, Target);
}

void USmoothSqlFunctionLibrary::K2_BindQueryParam_Float(USqliteStatement* Target, const FString& Param, float Value)
{
	Bind<float>(Param, Value, Target);
}

void USmoothSqlFunctionLibrary::K2_BindQueryParam_String(USqliteStatement* Target, const FString& Param,
                                                         const FString& Value)
{
	Bind<FString>(Param, Value, Target);
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int32 USmoothSqlFunctionLibrary::GetInt(USqliteStatement* Target, const FString& Column)
{
	return GetFromStatement<int32>(Target, Column);
}

int64 USmoothSqlFunctionLibrary::GetInt64(USqliteStatement* Target, const FString& Column)
{
	return GetFromStatement<int64>(Target, Column);
}

float USmoothSqlFunctionLibrary::GetFloat(USqliteStatement* Target, const FString& Column)
{
	return GetFromStatement<float>(Target, Column);
}

FString USmoothSqlFunctionLibrary::GetString(USqliteStatement* Target, const FString& Column)
{
	return GetFromStatement<FString>(Target, Column);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int32 USmoothSqlFunctionLibrary::GetInt_Column(FSqliteColumn& Column)
{
	return GetFromStructColumn<int32>(Column);
}

int64 USmoothSqlFunctionLibrary::GetInt64_Column(FSqliteColumn& Column)
{
	return GetFromStructColumn<int64>(Column);
}

float USmoothSqlFunctionLibrary::GetFloat_Column(FSqliteColumn& Column)
{
	return GetFromStructColumn<float>(Column);
}

FString USmoothSqlFunctionLibrary::GetString_Column(FSqliteColumn& Column)
{
	return GetFromStructColumn<FString>(Column);
}

bool USmoothSqlFunctionLibrary::IsValid_Column(FSqliteColumn& Column)
{
	return Column.Column.IsValid();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



USqliteStatement* USmoothSqlFunctionLibrary::K2_StepStatement(USqliteStatement* Target, bool& Success)
{
	if (Target)
	{
		Success = Target->Fetch();
	}

	return Target;
}

USqliteDatabase* USmoothSqlFunctionLibrary::GetDatabase(const FSqliteDBConnectionParms& Parms)
{
	auto DB = NewObject<USqliteDatabase>();
	if (DB->InitConnection(Parms))
	{
		return DB;
	}

	DB->MarkPendingKill();
	return nullptr;
}
