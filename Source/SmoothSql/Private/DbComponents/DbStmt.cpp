// Fill out your copyright notice in the Description page of Project Settings.


#include "DbComponents/DbStmt.h"

#include "DbComponents/DbObject.h"

void UDbStmt::Init(SQLite::Database* Db, const FString& SQL)
{
	bValid = false;
	SQLITE_TRY
	{
		if (Db)
		{
			// Init 
			RawStmt = MakeUnique<SQLite::Statement>(*Db, std::string(TCHAR_TO_UTF8(*SQL)));
			bValid = true;
		}
	}
	SQLITE_CATCH
	{
		Ctx.Log(L"Statement Preparing");
	}
	SQLITE_END

	if (!bValid)
	{
		ConditionalBeginDestroy();
	}
}

void UDbStmt::Release()
{
	RawStmt.Reset();
	bValid = false;
}

bool UDbStmt::IsDone() const
{
	if (DbStmtIsValid())
	{
		return RawStmt->isDone();
	}

	return false;
}

bool UDbStmt::Fetch()
{
	if (DbStmtIsValid())
	{
		SQLITE_TRY
		{
			return RawStmt->executeStep();
		}
		SQLITE_CATCH
		{
			Ctx.Log(L"Stmt Step Execution");
		}
		SQLITE_END
	}

	return false;
}

int32 UDbStmt::Execute()
{
	if (DbStmtIsValid())
	{
		SQLITE_TRY
		{
			return RawStmt->exec();
		}
		SQLITE_CATCH
		{
			Ctx.Log(L"Stmt Execution");
		}
		SQLITE_END
	}

	return false;
}

void UDbStmt::Reset()
{
	if (DbStmtIsValid())
	{
		SQLITE_TRY
		{
			RawStmt->reset();
		}
		SQLITE_CATCH
		{
			Ctx.Log(L"Stmt Reset");
		}
		SQLITE_END
	}
}

void UDbStmt::ClearBindings()
{
	if (DbStmtIsValid())
	{
		SQLITE_TRY
		{
			RawStmt->clearBindings();
		}
		SQLITE_CATCH
		{
			Ctx.Log(L"Stmt Clearing Bindings");
		}
		SQLITE_END
	}
}

void UDbStmt::Close()
{
	Release();
	ConditionalBeginDestroy();
}

SQLite::Statement* UDbStmt::Raw() const
{
	if (DbStmtIsValid())
	{
		return RawStmt.Get();
	}

	return nullptr;
}

TOptional<SQLite::Column> UDbStmt::GetColumn(int32 Idx)
{
	if (DbStmtIsValid())
	{
		SQLITE_TRY
		{
			auto Col = RawStmt->getColumn(Idx);
			return Col;
		}
		SQLITE_CATCH
		{
			Ctx.LogError(L"No such column: {0}", {Idx});
		}
		SQLITE_END
	}

	return {};
}

TOptional<SQLite::Column> UDbStmt::GetColumn(const FString& Col)
{
	if (DbStmtIsValid())
	{
		SQLITE_TRY
		{
			const auto cCol = TCHAR_TO_UTF8(*Col);
			return RawStmt->getColumn(cCol);
		}
		SQLITE_CATCH
		{
			Ctx.LogError( L"No such column: {0}", {Col});
		}
		SQLITE_END
	}

	return {};
}

void UDbStmt::BeginDestroy()
{
	Release();
	UObject::BeginDestroy();
}
