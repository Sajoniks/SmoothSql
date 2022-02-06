// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/K2Node_ForEachQueryResult.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_AssignmentStatement.h"
#include "K2Node_CallFunction.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_IfThenElse.h"
#include "KismetCompiler.h"
#include "SmoothSqlFunctionLibrary.h"
#include "DbComponents/DbStmt.h"

static const FName InnerLoopName = "Body";

UEdGraphPin* UK2Node_ForEachQueryResult::GetTargetPin() const
{
	return FindPinChecked(L"Target");
}


UEdGraphPin* UK2Node_ForEachQueryResult::GetLoopPin() const
{
	return FindPinChecked(InnerLoopName);
}

UEdGraphPin* UK2Node_ForEachQueryResult::GetCompletedPin() const
{
	return FindPinChecked(UEdGraphSchema_K2::PN_Completed);
}

void UK2Node_ForEachQueryResult::MovePinLinksOrCopyDefaults(FKismetCompilerContext& CompilerContext,
                                                            UEdGraphPin* Source, UEdGraphPin* Dest)
{
	if (Source->LinkedTo.Num( ) > 0)
	{
		CompilerContext.MovePinLinksToIntermediate( *Source, *Dest );
	}
	else
	{
		Dest->DefaultObject = Source->DefaultObject;
		Dest->DefaultValue = Source->DefaultValue;
		Dest->DefaultTextValue = Source->DefaultTextValue;
	}
}

void UK2Node_ForEachQueryResult::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	using GS = UEdGraphSchema_K2;
	
	CreatePin(EGPD_Input, GS::PC_Exec, GS::PN_Execute);
	CreatePin(EGPD_Input, GS::PC_Object, UDbStmt::StaticClass(), "Target");
	CreatePin(EGPD_Output, GS::PC_Exec, InnerLoopName);
	CreatePin(EGPD_Output, GS::PC_Object, UDbStmt::StaticClass(), "Statement");
	CreatePin(EGPD_Output, GS::PC_Exec, GS::PN_Completed);
}

void UK2Node_ForEachQueryResult::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// actions get registered under specific object-keys; the idea is that 
	// actions might have to be updated (or deleted) if their object-key is  
	// mutated (or removed)... here we use the node's class (so if the node 
	// type disappears, then the action should go with it)
	UClass* ActionKey = GetClass();
	// to keep from needlessly instantiating a UBlueprintNodeSpawner, first   
	// check to make sure that the registrar is looking for actions of this type
	// (could be regenerating actions for a specific asset, and therefore the 
	// registrar would only accept actions corresponding to that asset)
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_ForEachQueryResult::GetNodeTitle(ENodeTitleType::Type Title) const
{
	return FText::FromString("ForEach Query Result (Blocking)");
}

bool UK2Node_ForEachQueryResult::IsNodePure() const
{
	return false;
}

FSlateIcon UK2Node_ForEachQueryResult::GetIconAndTint(FLinearColor& OutColor) const
{
	static FSlateIcon Icon("EditorStyle", "GraphEditor.Macro.Loop_16x");
	return Icon;
}

FText UK2Node_ForEachQueryResult::GetMenuCategory() const
{
	return FText::FromString("SmoothSqlite|Statement");
}

FLinearColor UK2Node_ForEachQueryResult::GetNodeTitleColor() const
{
	return FColor(106, 90, 205);
}

void UK2Node_ForEachQueryResult::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	
	if (GetTargetPin()->LinkedTo.Num() == 0)
	{
		CompilerContext.MessageLog.Error(L"@@ Must have connected Statement to it", GetTargetPin());
		BreakAllNodeLinks();
	}
	
	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	
	UK2Node_IfThenElse* Branch = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
	Branch->AllocateDefaultPins();
	
	UK2Node_ExecutionSequence* Sequence = CompilerContext.SpawnIntermediateNode<UK2Node_ExecutionSequence>(this, SourceGraph);
	Sequence->AllocateDefaultPins();
	
	UK2Node_CallFunction* Step = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph); 
	Step->FunctionReference.SetExternalMember("K2_StepStatement", USmoothSqlFunctionLibrary::StaticClass());
	Step->AllocateDefaultPins();
	
	Schema->TryCreateConnection(Step->GetThenPin(), Branch->GetExecPin());
	Schema->TryCreateConnection(Step->FindPinChecked(L"Success"), Branch->GetConditionPin());
	Schema->TryCreateConnection(Branch->GetThenPin(), Sequence->GetExecPin());
	Schema->TryCreateConnection(Sequence->GetThenPinGivenIndex(1), Step->GetExecPin());
	
	MovePinLinksOrCopyDefaults(CompilerContext, GetExecPin(), Step->GetExecPin());
	MovePinLinksOrCopyDefaults(CompilerContext, GetLoopPin(), Sequence->GetThenPinGivenIndex(0));
	MovePinLinksOrCopyDefaults(CompilerContext, GetCompletedPin(), Branch->GetElsePin());
	MovePinLinksOrCopyDefaults(CompilerContext, GetTargetPin(), Step->FindPinChecked(L"Target"));
	MovePinLinksOrCopyDefaults(CompilerContext, FindPinChecked(L"Statement"), Step->GetReturnValuePin());
	
	BreakAllNodeLinks();
}

FText UK2Node_ForEachQueryResult::GetTooltipText() const
{
	return FText::FromString("Iterate over query results");
}
