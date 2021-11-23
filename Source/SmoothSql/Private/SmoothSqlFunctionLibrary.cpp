// Fill out your copyright notice in the Description page of Project Settings.


#include "SmoothSqlFunctionLibrary.h"

#include "SmoothSql.h"
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
	if (Row.ColumnPtr.IsValid())
	{
		try
		{
			auto Value = details::GetFromColumn<T>(*Row.ColumnPtr.Get());
			return Value;
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
static T GetFromStatement(FDbStatement& Statement, const FString& Column)
{
	if (Statement.IsValid())
	{
		auto Col = Statement.Get(Column);
		if (Col.IsSet())
		{
			return details::GetFromColumn<T>(Col.GetValue());
		}			
	}
	
	return T{};
	
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
		Statement.bind(Param,  std::string(TCHAR_TO_UTF8(*Value)));
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
void Bind(const FString& Param, const T& Value, FDbStatement& Statement)
{
	try
	{
		if (!Statement.IsValid()) return;

		const std::string ParamName = std::string(TCHAR_TO_UTF8(*Param));
			
		details::BindQueryParam(":" + ParamName, Value, *Statement.Raw());
	}
	catch (SQLite::Exception& e)
	{
		(void)e;
		details::Log(e, Value, Param);
	}
}

void USmoothSqlFunctionLibrary::K2_BindQueryParam_Int(FDbStatement& Target, const FString& Param, int32 Value)
{
	Bind<int32>(Param, Value, Target);
}

void USmoothSqlFunctionLibrary::K2_BindQueryParam_Int64(FDbStatement& Target, const FString& Param, int64 Value)
{
	Bind<int64>(Param, Value, Target);
}

void USmoothSqlFunctionLibrary::K2_BindQueryParam_Float(FDbStatement& Target, const FString& Param, float Value)
{
	Bind<float>(Param, Value, Target);
}

void USmoothSqlFunctionLibrary::K2_BindQueryParam_String(FDbStatement& Target, const FString& Param,
                                                         const FString& Value)
{
	Bind<FString>(Param, Value, Target);
}

void USmoothSqlFunctionLibrary::K2_BindQueryParam_Text(FDbStatement&  Target, const FString& Param,
	const FText& Value)
{
	K2_BindQueryParam_String(Target, Param, Value.ToString());
}

void USmoothSqlFunctionLibrary::K2_BindQueryParam_Name(FDbStatement& Target, const FString& Param,
	const FName& Value)
{
	K2_BindQueryParam_String(Target, Param, Value.ToString());
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int32 USmoothSqlFunctionLibrary::GetInt(FDbStatement& Target, const FString& Column)
{
	return GetFromStatement<int32>(Target, Column);
}

int64 USmoothSqlFunctionLibrary::GetInt64(FDbStatement& Target, const FString& Column)
{
	return GetFromStatement<int64>(Target, Column);
}

float USmoothSqlFunctionLibrary::GetFloat(FDbStatement& Target, const FString& Column)
{
	return GetFromStatement<float>(Target, Column);
}

FString USmoothSqlFunctionLibrary::GetString(FDbStatement& Target, const FString& Column)
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
	return Column.ColumnPtr != nullptr;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



FDbStatement& USmoothSqlFunctionLibrary::K2_StepStatement(FDbStatement& Target, bool& Success)
{
	if (Target.IsValid())
	{
		Success = Target.Fetch();
	}

	return Target;
}

FDbConnectionHandle USmoothSqlFunctionLibrary::OpenDbConnection(const FSqliteDBConnectionParms& Parms)
{
	return FDbConnectionHandle{Parms};
}

FDbStatement USmoothSqlFunctionLibrary::Query(FDbConnectionHandle& Handle, const FString& Query)
{
	return Handle.Query(Query);
}

bool USmoothSqlFunctionLibrary::IsValid_DbHandle(const FDbConnectionHandle& Handle)
{
	return Handle.IsValid();
}

bool USmoothSqlFunctionLibrary::IsValid_DbTransaction(const FDbConnectionHandle& Handle)
{
	return Handle.IsValidTransaction();
}

bool USmoothSqlFunctionLibrary::BeginTransaction(FDbConnectionHandle& Handle)
{
	return Handle.BeginTransaction();
}

bool USmoothSqlFunctionLibrary::CommitTransaction(FDbConnectionHandle& Handle)
{
	return Handle.CommitTransaction();
}

bool USmoothSqlFunctionLibrary::RollbackTransaction(FDbConnectionHandle& Handle)
{
	return Handle.RollbackTransaction();
}

bool USmoothSqlFunctionLibrary::Execute(FDbConnectionHandle& Handle, const FString& Query)
{
	return Handle.Execute(Query);
}

FSqliteColumn USmoothSqlFunctionLibrary::Fetch(FDbConnectionHandle& Handle, const FString& Query)
{
	return Handle.Fetch(Query);
}

bool USmoothSqlFunctionLibrary::IsValid_DbStatement(const FDbStatement& Handle)
{
	return Handle.IsValid();
}

bool USmoothSqlFunctionLibrary::IsDone(const FDbStatement& Handle)
{
	return Handle.IsDone();
}

int32 USmoothSqlFunctionLibrary::Execute_Statement(FDbStatement& Statement)
{
	return Statement.Execute();
}

bool USmoothSqlFunctionLibrary::Fetch_Statement(FDbStatement& Statement)
{
	return Statement.Fetch();
}

void USmoothSqlFunctionLibrary::ClearBinds(FDbStatement& Statement)
{
	Statement.ClearBinds();
}

void USmoothSqlFunctionLibrary::Reset(FDbStatement& Statement)
{
	Statement.Reset();
}

void USmoothSqlFunctionLibrary::CloseStatement(FDbStatement& Statement)
{
	Statement.Invalidate();
}

void USmoothSqlFunctionLibrary::CloseConnection(FDbConnectionHandle& Handle)
{
	Handle.Invalidate();
}


