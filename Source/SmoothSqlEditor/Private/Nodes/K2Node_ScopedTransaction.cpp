// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/K2Node_ScopedTransaction.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_AssignmentStatement.h"
#include "K2Node_CallFunction.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_TemporaryVariable.h"
#include "K2Node_VariableSet.h"
#include "KismetCompiler.h"
#include "SmoothSqlFunctionLibrary.h"
#include "Data/SmoothSqliteDataTypes.h"
#include "DbComponents/DbObject.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

static FName TargetPinName = L"Connection";
static FName CreateNewConnectionPinName = L"Create New Connection";
static FName DbConnectionParmsPinName = L"Connection Params";
static FName DbConnectionOutPinName = L"Scoped Connection";
static FName ScopeExitPinName = L"Scope Completed";


void MovePinLinksOrCopyDefaults(FKismetCompilerContext& CompilerContext, UEdGraphPin* Source,
	UEdGraphPin* Dest)
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


UEdGraphPin* UK2Node_ScopedTransaction::GetConditionCreatePin() const
{
	return FindPinChecked(CreateNewConnectionPinName);
}

UEdGraphPin* UK2Node_ScopedTransaction::GetInnerDbHandlePin() const
{
	return FindPinChecked(DbConnectionOutPinName);
}

UEdGraphPin* UK2Node_ScopedTransaction::GetConnectionParamsPin() const
{
	return FindPin(DbConnectionParmsPinName);
}

UEdGraphPin* UK2Node_ScopedTransaction::GetThenPin() const
{
	return FindPinChecked(UEdGraphSchema_K2::PN_Then);
}

UEdGraphPin* UK2Node_ScopedTransaction::GetDbHandlePin() const
{
	return FindPinChecked(TargetPinName);
}

UEdGraphPin* UK2Node_ScopedTransaction::GetScopeExitPin() const
{
	return FindPinChecked(ScopeExitPinName);
}

void UK2Node_ScopedTransaction::UpdateConnectionPin()
{
	// if (!GetConditionCreatePin()->HasAnyConnections() && GetConditionCreatePin()->DefaultValue == "true")
	// {
	// 	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FSqliteDBConnectionParms::StaticStruct(), DbConnectionParmsPinName);
	//
	// 	if (const auto TargetPin = FindPin(TargetPinName))
	// 	{
	// 		RemovePin(TargetPin);	
	// 	}
	// }
	// else
	// {
	// 	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FDbConnectionHandle::StaticStruct(), TargetPinName);
	// 	
	// 	if (const auto ParamsPin = FindPin(DbConnectionParmsPinName))
	// 	{
	// 		RemovePin(ParamsPin);	
	// 	}
	// }
	//
	// if (auto Graph = GetGraph())
	// {
	// 	Graph->NotifyGraphChanged();
	// }
}

void UK2Node_ScopedTransaction::AllocateDefaultPins()
{
	using GS = UEdGraphSchema_K2;

	CreatePin(EGPD_Input, GS::PC_Exec, GS::PN_Execute);
	// CreatePin(EGPD_Input, GS::PC_Boolean, CreateNewConnectionPinName);
	CreatePin(EGPD_Input, GS::PC_Struct, FSqliteDBConnectionParms::StaticStruct(), DbConnectionParmsPinName);
	CreatePin(EGPD_Output, GS::PC_Exec, GS::PN_Then);
	CreatePin(EGPD_Output, GS::PC_Object, UDbObject::StaticClass(), DbConnectionOutPinName);
	CreatePin(EGPD_Output, GS::PC_Exec, ScopeExitPinName);
}

FText UK2Node_ScopedTransaction::GetNodeTitle(ENodeTitleType::Type Title) const
{
	return FText::FromString(L"Scoped Transaction");
}

bool UK2Node_ScopedTransaction::IsNodePure() const
{
	return false;
}

FText UK2Node_ScopedTransaction::GetTooltipText() const
{
	return Super::GetTooltipText();
}

FLinearColor UK2Node_ScopedTransaction::GetNodeTitleColor() const
{
	return FColor(228, 208, 10);
}

FText UK2Node_ScopedTransaction::GetKeywords() const
{
	return Super::GetKeywords();
}

FText UK2Node_ScopedTransaction::GetMenuCategory() const
{
	return Super::GetMenuCategory();
}

void UK2Node_ScopedTransaction::ReconstructNode()
{
	Super::ReconstructNode();

	// UpdateConnectionPin();
}

void UK2Node_ScopedTransaction::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);

	// if (Pin == GetConditionCreatePin())
	// {
	// 	UpdateConnectionPin();
	// }
}

void UK2Node_ScopedTransaction::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);
	
// 	if (Pin == GetConditionCreatePin())
// 	{
// 		UpdateConnectionPin();
// 	}
}

void UK2Node_ScopedTransaction::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
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

void UK2Node_ScopedTransaction::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	
	auto Schema = CompilerContext.GetSchema();
	
	UK2Node_ExecutionSequence* Sequence = CompilerContext.SpawnIntermediateNode<UK2Node_ExecutionSequence>(this, SourceGraph);
	Sequence->AllocateDefaultPins();
	
	UK2Node_CallFunction* CallBeginFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallBeginFunction->FunctionReference.SetExternalMember(L"BeginTransaction", USmoothSqlFunctionLibrary::StaticClass());
	CallBeginFunction->AllocateDefaultPins();

	UK2Node_CallFunction* CallCommitFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallCommitFunction->FunctionReference.SetExternalMember(L"CommitTransaction", USmoothSqlFunctionLibrary::StaticClass());
	CallCommitFunction->AllocateDefaultPins();
	
	UK2Node_CallFunction* CallCreateConnection = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallCreateConnection->FunctionReference.SetExternalMember(L"OpenDbConnection", USmoothSqlFunctionLibrary::StaticClass());
	CallCreateConnection->AllocateDefaultPins();
	
	
	// UK2Node_CallFunction* CallCompFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	// CallCompFunction->FunctionReference.SetExternalMember(L"EqualEqual_BoolBool", UKismetMathLibrary::StaticClass());
	// CallCompFunction->AllocateDefaultPins();

	UK2Node_TemporaryVariable* TempVar = CompilerContext.SpawnIntermediateNode<UK2Node_TemporaryVariable>(this, SourceGraph);
	TempVar->VariableType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	TempVar->VariableType.PinSubCategoryObject = UDbObject::StaticClass();
	TempVar->AllocateDefaultPins();

	UK2Node_AssignmentStatement* TempVarAssign = CompilerContext.SpawnIntermediateNode<UK2Node_AssignmentStatement>(this, SourceGraph);
	TempVarAssign->AllocateDefaultPins();

	// bool bWillCreateConnection = false;
	// // Route from OpenDbConnection
	// if (auto ParmsPin = GetConnectionParamsPin())
	// {
	// 	Schema->TryCreateConnection(TempVar->GetVariablePin(), TempVarAssign->GetVariablePin());
	// 	Schema->TryCreateConnection(CallCreateConnection->GetReturnValuePin(), TempVarAssign->GetValuePin());
	// 	MovePinLinksOrCopyDefaults(CompilerContext, ParmsPin, CallCreateConnection->FindPinChecked(L"Parms"));
	// 	bWillCreateConnection = true;
	// }
	// else // Route from input
	// {
	// 	Schema->TryCreateConnection(TempVar->GetVariablePin(), TempVarAssign->GetVariablePin());
	// 	MovePinLinksOrCopyDefaults(CompilerContext, GetDbHandlePin(), TempVarAssign->GetValuePin());
	// }

	// If need to craete connection
	// UK2Node_IfThenElse* Branch = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
	// Branch->AllocateDefaultPins();

	// // From comparison to branch condition
	// Schema->TryCreateConnection(CallCompFunction->GetReturnValuePin(), Branch->GetConditionPin());
	//
	// // --> Create connection
	// MovePinLinksOrCopyDefaults(CompilerContext, GetConditionCreatePin(), CallCompFunction->FindPinChecked(L"A"));
	// // --> True
	// CallCompFunction->FindPinChecked(L"B")->DefaultValue = "true";

	// --> To Branch
	Schema->TryCreateConnection(Sequence->GetThenPinGivenIndex(0), CallCreateConnection->GetExecPin());
	// --> To Commit
	Schema->TryCreateConnection(Sequence->GetThenPinGivenIndex(1), CallCommitFunction->GetExecPin());
	Schema->TryCreateConnection(CallCreateConnection->GetThenPin(), TempVarAssign->GetExecPin());

	Schema->TryCreateConnection(TempVar->GetVariablePin(), TempVarAssign->GetVariablePin());
	Schema->TryCreateConnection(CallCreateConnection->GetReturnValuePin(), TempVarAssign->GetValuePin());
	
	// --> To Begin Commit
	Schema->TryCreateConnection(TempVarAssign->GetThenPin(), CallBeginFunction->GetExecPin());
	Schema->TryCreateConnection(TempVar->GetVariablePin(), CallBeginFunction->FindPinChecked(L"Handle"));
	Schema->TryCreateConnection(TempVar->GetVariablePin(), CallCommitFunction->FindPinChecked(L"Handle"));

	MovePinLinksOrCopyDefaults(CompilerContext, GetConnectionParamsPin(), CallCreateConnection->FindPinChecked(L"Parms"));
	MovePinLinksOrCopyDefaults(CompilerContext, GetInnerDbHandlePin(), TempVar->GetVariablePin());
	MovePinLinksOrCopyDefaults(CompilerContext, GetExecPin(), Sequence->GetExecPin());
	MovePinLinksOrCopyDefaults(CompilerContext, GetThenPin(), CallBeginFunction->GetThenPin());
	MovePinLinksOrCopyDefaults(CompilerContext, GetScopeExitPin(), CallCommitFunction->GetThenPin());

	BreakAllNodeLinks();
}
