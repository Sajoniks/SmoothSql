// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/SmoothSqliteDataTypes.h"
#include "SQLiteCpp/Backup.h"
#include "UObject/NoExportTypes.h"
#include "DbObject.generated.h"

class UDbStmt;
/**
 * 
 */
UCLASS(BlueprintType)
class SMOOTHSQL_API UDbObject : public UObject
{
	GENERATED_BODY()

	// This object can be created only with this function library
	friend class USmoothSqlFunctionLibrary;

	
	/**
	 * @brief Initialize underlying SQLite::Database object
	 */
	void Init(int32 OpenFlags);

	/**
	 *
	 */
	void Release();
	
public:

	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Database|Get")
	int32 GetChangesNum() const;


	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Database|Get", meta=(DisplayName="Connection Is Valid"))
	bool DbObjectIsValid() const { return bValid; }

	static bool DbObjectIsValid(const UDbObject* Object)
	{
		return IsValid(Object) && Object->bValid;
	}

	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Database|Get", meta=(DisplayName="Transaction Active"))
	bool DbTransactIsValid() const { return Transaction.IsValid(); }	

	
	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Database|Action")
	void Close();

	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Database|Action")
	UDbStmt* Prepare(const FString& SQL);

	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Database|Action")
	int32 Execute(const FString& SQL);

	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Database|Action")
	bool Fetch(const FString& SQL, FSqliteColumn& Column);

	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Database|Action")
	bool StartDbTransaction(EDbTransactionFlags Flags = EDbTransactionFlags::Deferred);

	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Database|Action")
	bool CommitDbTransaction();

	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Database|Action")
	void RollbackDbTransaction();

	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Database|Action")
	void MakeBackup();


	UFUNCTION(BlueprintCallable, Category="SmoothSql|Database|Get")
	bool IsBusy() const;
	
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Database|Get")
	bool IsLocked() const;
	
	/**
	 *
	 */
	virtual void BeginDestroy() override;

private:

	bool bValid;						///< Can this object be used safely
	FSqliteDBConnectionParms DbParams;	///< Parameters of wrapped db
	
	TUniquePtr<SQLite::Database> RawDb;				///< Raw SQLite database object
	TUniquePtr<SQLite::Transaction> Transaction;	///< Current transaction (if any)
};
