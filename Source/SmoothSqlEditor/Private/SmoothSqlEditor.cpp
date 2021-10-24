// Copyright Epic Games, Inc. All Rights Reserved.

#include "SmoothSqlEditor.h"
#include "Core.h"
#include "Modules/ModuleManager.h"


	
IMPLEMENT_MODULE(FSmoothSqlEditorModule, SmoothSqlEditor)

void FSmoothSqlEditorModule::StartupModule()
{
	IModuleInterface::StartupModule();
}

void FSmoothSqlEditorModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
}


#undef LOCTEXT_NAMESPACE