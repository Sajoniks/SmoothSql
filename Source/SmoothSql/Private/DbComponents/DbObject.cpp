// Fill out your copyright notice in the Description page of Project Settings.


#include "DbComponents/DbObject.h"
#include "SmoothSql.h"

#include "DbDefaultSettings.h"
#include "DbComponents/DbStmt.h"

void UDbObject::Init(int32 OpenFlags)
{
	bValid = false;
	if (const auto Settings = GetDefault<UDbDefaultSettings>())
	{
		DbParams = Settings->DefaultConnectionParams;
	
		// Obtain path to project dir
		const auto GameDir = FPaths::ConvertRelativePathToFull( FPaths::ProjectDir() );
		const auto DBName = DbParams.DBName;
		const auto Folder = DbParams.Folder;

		// Make sure that path is valid
		check(!DBName.IsEmpty())
		check(!Folder.IsEmpty())
		
		// Make path to DB
		auto DBDir = FPaths::Combine(GameDir, Folder, DBName);
		DBDir = FPaths::SetExtension(DBDir, "db");

		
		// Try to open DB
		SQLITE_TRY
		{
			int32 Flags = 0;
			if (OpenFlags & SQLITE_GET_FLAG(EDbOpenFlags::ReadOnly))
				Flags |= SQLite::OPEN_READONLY;
			
			if (OpenFlags & SQLITE_GET_FLAG(EDbOpenFlags::ReadWrite))
				Flags |= SQLite::OPEN_READWRITE;
			
			if (OpenFlags & SQLITE_GET_FLAG(EDbOpenFlags::URI))
				Flags |= SQLite::OPEN_URI;
			
			if (OpenFlags & SQLITE_GET_FLAG(EDbOpenFlags::Create))
				Flags |= SQLite::OPEN_CREATE;
			
			if (OpenFlags & SQLITE_GET_FLAG( EDbOpenFlags::NoMutex))
				Flags |= SQLite::OPEN_NOMUTEX;
			
			if (OpenFlags & SQLITE_GET_FLAG(EDbOpenFlags::PrivateCache))
				Flags |= SQLite::OPEN_PRIVATECACHE;
			
			if (OpenFlags & SQLITE_GET_FLAG(EDbOpenFlags::SharedCache))
				Flags |= SQLite::OPEN_SHAREDCACHE;
			
			if (OpenFlags & SQLITE_GET_FLAG(EDbOpenFlags::Memory))
				Flags |= SQLite::OPEN_MEMORY;
			
			// Move
			RawDb = MakeUnique<SQLite::Database>(SQLite::Database(std::string(TCHAR_TO_UTF8(*DBDir)), Flags, DbParams.BusyTimeout));
			bValid = true;

			// Log
			Ctx.LogMsg( L"Opened database \"{0}\"", {DbParams.DBName});
		}
		SQLITE_CATCH
		{
			Ctx.Log(L"Database Opening");
		}
		SQLITE_END
	}

	if (!bValid)
	{
		ConditionalBeginDestroy();
		MarkPendingKill();
	}
}

int32 UDbObject::GetChangesNum() const
{
	if (DbObjectIsValid(this))
	{
		return RawDb->getChanges();
	}

	return -1;
}

void UDbObject::Release()
{
	RawDb.Release();
	bValid = false;
}

void UDbObject::Close()
{
	Release();
	MarkPendingKill();
	ConditionalBeginDestroy();
}

UDbStmt* UDbObject::Prepare(const FString& SQL)
{
	if (DbObjectIsValid(this))
	{
		if (auto Stmt = NewObject<UDbStmt>())
		{
			Stmt->Init(RawDb.Get(), SQL);
			if (!Stmt->DbStmtIsValid())
			{
				Stmt->ConditionalBeginDestroy();
				return nullptr;
			}

			return Stmt;
		}
	}
	else
	{
		UE_LOG(LogSmoothSqlite, Warning, L"Tried to access NULL DbObject!")
	}

	return nullptr;
}

int32 UDbObject::Execute(const FString& SQL)
{
	if (DbObjectIsValid(this))
	{
		SQLITE_TRY
		{
			const auto cSQL = std::string(TCHAR_TO_UTF8(*SQL));
			return RawDb->exec(cSQL);
		}
		SQLITE_CATCH
		{
			Ctx.Log(L"Db Statements Execution");
		}
		SQLITE_END
	}

	return -1;
}

bool UDbObject::Fetch(const FString& SQL, FSqliteColumn& Column)
{
	if (DbObjectIsValid(this))
	{
		SQLITE_TRY
		{
			const auto cSQL = std::string(TCHAR_TO_UTF8(*SQL));
			Column = RawDb->execAndGet(cSQL);
			return true;
		}
		SQLITE_CATCH
		{
			Ctx.Log(L"Db Fetch");
		}
		SQLITE_END
	}

	return false;
}

bool UDbObject::StartDbTransaction(EDbTransactionFlags Flags)
{
	if (DbObjectIsValid(this) && !DbTransactIsValid())
	{
		SQLITE_TRY
		{
			SQLite::TransactionBehavior Beh = SQLite::TransactionBehavior::DEFERRED;

			switch (Flags)
			{
			case EDbTransactionFlags::Immediate:
				Beh = SQLite::TransactionBehavior::IMMEDIATE; break;
			case EDbTransactionFlags::Deferred:
				Beh = SQLite::TransactionBehavior::DEFERRED; break;
			case EDbTransactionFlags::Exclusive:
				Beh = SQLite::TransactionBehavior::EXCLUSIVE; break;
			}
			
			Transaction = MakeUnique<SQLite::Transaction>(*RawDb, Beh);
			Ctx.LogMsg(L"Initiated Db Transaction, db: \"{0}\"", {DbParams.DBName});

			return true;
		}
		SQLITE_CATCH
		{
			Ctx.Log(L"Start Db Transaction");
		}
		SQLITE_END
	}

	return false;
}

bool UDbObject::CommitDbTransaction()
{
	bool bCommit = false;
	
	if (DbTransactIsValid())
	{
		SQLITE_TRY
		{
			Transaction->commit();
			Ctx.LogMsg(L"Db Transaction Commit Success, db: \"{0}\"", {DbParams.DBName});
			bCommit = true;
		}
		SQLITE_CATCH
		{
			Ctx.Log(L"Commiting Db Transaction");
			bCommit = false;
		}
		SQLITE_END
		
		Transaction.Release();
	}

	return bCommit;
}

void UDbObject::RollbackDbTransaction()
{
	Transaction.Release();
}

void UDbObject::MakeBackup()
{
	if (DbObjectIsValid(this))
	{
		SQLITE_TRY
		{
			// Obtain path to project dir
			const auto GameDir = FPaths::ConvertRelativePathToFull( FPaths::ProjectDir() );
			const auto DBName = DbParams.DBName + FString("-backup-") + FDateTime::Now().ToString(L"dmY-his");
			const auto Folder = DbParams.Folder;
			
			RawDb->backup(TCHAR_TO_ANSI(*FPaths::Combine(GameDir, Folder, "Backups", DBName)), SQLite::Database::BackupType::Save);

			Ctx.LogMsg(L"Performed Database Backup {0}/{1}/Backups/{2}", {GameDir, Folder, DBName});
		}
		SQLITE_CATCH
		{
			Ctx.Log(L"Db Backup");
		}
		SQLITE_END
	}
}

bool UDbObject::IsBusy() const
{
	if (DbObjectIsValid(this))
	{
		return RawDb->getErrorCode() == SQLITE_BUSY;
	}

	return false;
}

bool UDbObject::IsLocked() const
{
	if (DbObjectIsValid(this))
	{
		return RawDb->getErrorCode() == SQLITE_LOCKED;
	}

	return false;
}

void UDbObject::BeginDestroy()
{
	Release();
	UObject::BeginDestroy();
}
