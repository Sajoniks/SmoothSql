// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"


DECLARE_LOG_CATEGORY_EXTERN(LogSmoothSqlite, Display, Display);



#ifdef SQLITE_TRY
#undef SQLITE_TRY
#endif

#ifdef SQLITE_CATCH
#undef SQLITE_CATCH
#endif SQLITE_CATCH

#ifdef SQLITE_END
#undef SQLITE_END
#endif


#define SQLITE_TRY \
	{	\
		struct FSqliteTryCatchCtx \
		{	\
			int32 ErrorCode;\
			FString FunctionFrame;\
			FString ErrorMsg; \
		    \
			void LogError(const TCHAR* Message, const FStringFormatOrderedArguments& Args)\
			{\
				FunctionFrame = FString(ANSI_TO_TCHAR(__FUNCTION__)).LeftChop(FString(L"::FSqliteTryCatchCtx::LogError").Len());\
				SET_WARN_COLOR(COLOR_RED)\
				GLog->Logf(ELogVerbosity::Error, L"[SmoothSql][Error] SQLite Reporting Exception <<< %s\n\t>>> At:%s, line %d", *FString::Format(Message, Args), *FunctionFrame, __LINE__);\
				CLEAR_WARN_COLOR()\
			}\
			void LogMsg(const TCHAR* Message, const FStringFormatOrderedArguments& Args)\
			{\
				FunctionFrame = FString(ANSI_TO_TCHAR(__FUNCTION__)).LeftChop(FString(L"::FSqliteTryCatchCtx::LogMsg").Len());\
				SET_WARN_COLOR(COLOR_GREEN)\
				GLog->Logf(ELogVerbosity::Display, L"[SmoothSql][Msg] SQLite Reporting <<< %s\n\t>>> At:%s, line %d", *FString::Format(Message, Args), *FunctionFrame, __LINE__);\
				CLEAR_WARN_COLOR()\
			}\
			void Log(const TCHAR* Intent)\
			{\
				FunctionFrame = FString(ANSI_TO_TCHAR(__FUNCTION__)).LeftChop(FString(L"::FSqliteTryCatchCtx::Log").Len());\
				SET_WARN_COLOR(COLOR_RED)\
				GLog->Logf(ELogVerbosity::Error, L"[SmoothSql][Error] SQLite Reporting Exception while %s\n\t<<< At:%s, line %d\n\t>>> SQLite message: \"%s (%d)\"", Intent, *FunctionFrame, __LINE__, *ErrorMsg, ErrorCode);\
				CLEAR_WARN_COLOR()\
			}\
		} Ctx;\
		try {\

#define SQLITE_CATCH \
	} catch (SQLite::Exception& e) { \
		Ctx.ErrorCode = e.getErrorCode(); \
		Ctx.ErrorMsg = FString(UTF8_TO_TCHAR(e.getErrorStr()));

#define SQLITE_END \
	} }

class FSmoothSqlModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

};
