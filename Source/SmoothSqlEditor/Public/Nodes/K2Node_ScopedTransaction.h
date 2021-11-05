// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "Data/SmoothSqliteDataTypes.h"
#include "K2Node_ScopedTransaction.generated.h"

/**
 * 
 */
UCLASS()
class SMOOTHSQLEDITOR_API UK2Node_ScopedTransaction : public UK2Node
{
	GENERATED_BODY()

	UEdGraphPin* GetConditionCreatePin() const;
	UEdGraphPin* GetInnerDbHandlePin() const;
	UEdGraphPin* GetConnectionParamsPin() const;
	UEdGraphPin* GetThenPin() const;
	UEdGraphPin* GetDbHandlePin() const;
	UEdGraphPin* GetScopeExitPin() const;

	void UpdateConnectionPin();

public:
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool IsNodePure() const override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetKeywords() const override;
	virtual FText GetMenuCategory() const override;
	virtual void ReconstructNode() override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

private:
};
