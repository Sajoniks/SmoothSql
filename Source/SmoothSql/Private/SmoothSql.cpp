// Copyright Epic Games, Inc. All Rights Reserved.

#include "SmoothSql.h"
#include "Core.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogSmoothSqlite)

void FSmoothSqlModule::StartupModule()
{
#if WITH_EDITOR
	FModuleManager::Get().LoadModuleChecked("SmoothSqlEditor");
#endif
}

void FSmoothSqlModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSmoothSqlModule, SmoothSql)
