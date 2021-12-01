// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/SmoothSqliteDataTypes.h"
#include "Engine/DeveloperSettings.h"
#include "DbDefaultSettings.generated.h"

/**
 * 
 */
UCLASS(Config="Game", meta=(DisplayName="Database Settings"))
class UDbDefaultSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UDbDefaultSettings() = default;

	// Parameters that are used when connection is being opened 'in place'
	UPROPERTY(Config, EditAnywhere, Category="General")
	FSqliteDBConnectionParms DefaultConnectionParams;
	
};
