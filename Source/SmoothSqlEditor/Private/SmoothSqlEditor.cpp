// Copyright Epic Games, Inc. All Rights Reserved.

#include "SmoothSql.h"
#include "Core.h"
#include "Modules/ModuleManager.h"


void FSmoothSqlEditorModule::StartupModule()
{
}

void FSmoothSqlEditorModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSmoothSqlEditorModule, SmoothSqlEditor)
