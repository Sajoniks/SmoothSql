// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SQLiteCpp/Column.h"
#include "UObject/NoExportTypes.h"
#include "SQLiteCpp/Statement.h"
#include "DbStmt.generated.h"


namespace SQLite
{
	class Statement;
	class Database;
}

/**
 * 
 */
UCLASS(BlueprintType)
class SMOOTHSQL_API UDbStmt : public UObject
{

	friend class UDbObject;
	
	GENERATED_BODY()


	/**
	 * @brief 
	 */
	void Init(class SQLite::Database* Db, const FString& SQL);

	/**
	 *
	 */
	void Release();
public:

	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Statement|Get", meta=(DisplayName="Statement Is Valid"))
	bool DbStmtIsValid() const { return bValid; }

	static bool DbStmtIsValid(UDbStmt* Stmt)
	{
		return IsValid(Stmt) && Stmt->DbStmtIsValid();
	} 
	
	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Statement|Get")
	bool IsDone() const;

	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Statement|Action")
	bool Fetch();

	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Statement|Action")
	int32 Execute();

	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Statement|Action")
	void Reset();

	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Statement|Action")
	void ClearBindings();

	/**
	 *
	 */
	UFUNCTION(BlueprintCallable, Category="SmoothSql|Statement|Action")
	void Close();

	/**
	 *
	 */
	SQLite::Statement* Raw() const;

	/**
	 *
	 */
	TOptional<SQLite::Column> GetColumn(int32 Idx);

	/**
	 *
	 */
	TOptional<SQLite::Column> GetColumn(const FString& Col);

	/**
	 *
	 */
	virtual void BeginDestroy() override;

private:

	bool bValid;	///< Is statement valid
	TUniquePtr<SQLite::Statement> RawStmt;		///< Raw SQLite Statement object
};
