// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/K2Node_BindQueryParam.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"
#include "SmoothSqlFunctionLibrary.h"
#include "DbComponents/DbStmt.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/CompilerResultsLog.h"

const static FName ParamPinName = "ParamName";
const static FName ValuePinName = "ValueName";

UEdGraphPin* UK2Node_BindQueryParam::GetValuePin() const
{
	return FindPinChecked(ValuePinName);
}

UEdGraphPin* UK2Node_BindQueryParam::GetParamPin() const
{
	return FindPinChecked(ParamPinName);
}

UEdGraphPin* UK2Node_BindQueryParam::GetTargetPin() const
{
	return FindPinChecked(FName("Target"));
}

void UK2Node_BindQueryParam::MovePinLinksOrCopyDefaults(FKismetCompilerContext& CompilerContext, UEdGraphPin* Source,
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

bool IsExecPin(const UEdGraphPin* Pin)
{
	return Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec;
}

void UK2Node_BindQueryParam::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UDbStmt::StaticClass(), "Target");
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, ParamPinName);
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, ValuePinName);
}

FText UK2Node_BindQueryParam::GetNodeTitle(ENodeTitleType::Type Title) const
{
	return FText::FromString("Bind Query Param");
}

bool UK2Node_BindQueryParam::IsNodePure() const
{
	return false;
}

FText UK2Node_BindQueryParam::GetTooltipText() const
{
	return FText::FromString("Bind param value to SqliteStatement");
}

FLinearColor UK2Node_BindQueryParam::GetNodeTitleColor() const
{
	return FColor(255, 239, 213);
}

FText UK2Node_BindQueryParam::GetKeywords() const
{
	return FText::FromString("Bind, Param, Query, Database, Value, Statement, Parameter");
}

bool UK2Node_BindQueryParam::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin,
	FString& OutReason) const
{
	const auto ParamPin = GetValuePin();
	if (MyPin->Direction == EGPD_Input && MyPin == ParamPin)
	{
		const auto OtherPinCategory = OtherPin->PinType.PinCategory;

		using GS = UEdGraphSchema_K2;
		
		if
		(
				OtherPinCategory == GS::PC_Int
			||	OtherPinCategory == GS::PC_Int64
			||	OtherPinCategory == GS::PC_String
			||	OtherPinCategory == GS::PC_Text
			||	OtherPinCategory == GS::PC_Name
		)
		{
			return false;
		}
		else
		{
			OutReason = "Can bind only Int, Int64, String, Text and Name";
			return true;
		}
	}
	
	return Super::IsConnectionDisallowed(MyPin, OtherPin, OutReason);
}

FText UK2Node_BindQueryParam::GetMenuCategory() const
{
	return FText::FromString("SmoothSqlite|Statement");
}

void UK2Node_BindQueryParam::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
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

void UK2Node_BindQueryParam::SyncParamType(UEdGraphPin* Pin)
{
	if (Pin == GetValuePin())
	{
		bool bPinTypeChanged = false;
		if (Pin->LinkedTo.Num() == 0)
		{
			static const FEdGraphPinType WildcardPinType = FEdGraphPinType(UEdGraphSchema_K2::PC_Wildcard, NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType());

			if (Pin->PinType != WildcardPinType)
			{
				Pin->PinType = WildcardPinType;
				bPinTypeChanged = true;
			}
		}
		else
		{
			auto ArgSourcePin = Pin->LinkedTo[0];

			if (Pin->PinType != ArgSourcePin->PinType)
			{
				Pin->PinType = ArgSourcePin->PinType;
				bPinTypeChanged = true;
			}
		}

		if (bPinTypeChanged)
		{
			GetGraph()->NotifyGraphChanged();

			auto BP = GetBlueprint();
			if (!BP->bBeingCompiled)
			{
				FBlueprintEditorUtils::MarkBlueprintAsModified(BP);
				BP->BroadcastChanged();
			}
		}
	}
}

void UK2Node_BindQueryParam::ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);
}

void UK2Node_BindQueryParam::PinTypeChanged(UEdGraphPin* Pin)
{
	Super::PinTypeChanged(Pin);
	SyncParamType(Pin);
}

void UK2Node_BindQueryParam::ReconstructNode()
{
	Super::ReconstructNode();
	SyncParamType(GetValuePin());
}

void UK2Node_BindQueryParam::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);
	SyncParamType(Pin);
}

void UK2Node_BindQueryParam::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();

	if (GetTargetPin()->LinkedTo.Num() == 0)
	{
		CompilerContext.MessageLog.Error(L"Target must be specified for @@", GetTargetPin());
		BreakAllNodeLinks();
		return;
	}

	if (GetValuePin()->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
	{
		CompilerContext.MessageLog.Error(L"Wildcard can't be bound: @@", GetValuePin());
		BreakAllNodeLinks();
		return;
	}

	// Create bind function intermediate
	UK2Node_CallFunction* CallBindFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);

	// Pick valid convert function
	{
		using GS = UEdGraphSchema_K2;
		const auto PinCategory = GetValuePin()->PinType.PinCategory;

		if (PinCategory == GS::PC_Int)
		{
			CallBindFunction->FunctionReference.SetExternalMember("K2_BindQueryParam_Int", USmoothSqlFunctionLibrary::StaticClass());
		}
		else if (PinCategory == GS::PC_Int64)
		{
			CallBindFunction->FunctionReference.SetExternalMember("K2_BindQueryParam_Int64", USmoothSqlFunctionLibrary::StaticClass());
		}
		else if (PinCategory == GS::PC_Float)
		{
			CallBindFunction->FunctionReference.SetExternalMember("K2_BindQueryParam_Float", USmoothSqlFunctionLibrary::StaticClass());
		}
		else if (PinCategory == GS::PC_String)
		{
			CallBindFunction->FunctionReference.SetExternalMember("K2_BindQueryParam_String", USmoothSqlFunctionLibrary::StaticClass());
		}
		else if (PinCategory == GS::PC_Text)
		{
			CallBindFunction->FunctionReference.SetExternalMember("K2_BindQueryParam_Text", USmoothSqlFunctionLibrary::StaticClass());
		}
		else if (PinCategory == GS::PC_Name)
		{
			CallBindFunction->FunctionReference.SetExternalMember("K2_BindQueryParam_Name", USmoothSqlFunctionLibrary::StaticClass());
		}
		else
		{
			check(false && "Invalid Pin Category");
		}
		

		CallBindFunction->AllocateDefaultPins();

		// Move to intermediate
		MovePinLinksOrCopyDefaults(CompilerContext, GetTargetPin(), CallBindFunction->FindPinChecked(L"Target"));
		MovePinLinksOrCopyDefaults(CompilerContext, GetValuePin(), CallBindFunction->FindPinChecked(L"Value"));
		MovePinLinksOrCopyDefaults(CompilerContext, GetParamPin(), CallBindFunction->FindPinChecked(L"Param"));

		MovePinLinksOrCopyDefaults(CompilerContext, FindPinChecked(GS::PN_Execute), CallBindFunction->GetExecPin());
		MovePinLinksOrCopyDefaults(CompilerContext, FindPinChecked(GS::PN_Then), CallBindFunction->GetThenPin());
		
	
	}
	
	BreakAllNodeLinks();
}
