// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DeferredDatabaseCleanupAction.generated.h"

class USqliteDatabase;
/**
 * Actions that must be performed before database close
 */
UCLASS(BlueprintType, Blueprintable)
class UDeferredDatabaseCleanupAction : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "Database", meta=(DisplayName="! On Database Deferred Cleanup"))
	void OnDatabaseCleanup(USqliteDatabase* Database) const;
};
