// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_ForEachQueryResult.generated.h"

/**
 * 
 */
UCLASS()
class SMOOTHSQLEDITOR_API UK2Node_ForEachQueryResult : public UK2Node
{
	GENERATED_BODY()

	UEdGraphPin* GetTargetPin() const;
	UEdGraphPin* GetLoopPin() const;
	UEdGraphPin* GetCompletedPin() const;

	void MovePinLinksOrCopyDefaults( FKismetCompilerContext &CompilerContext, UEdGraphPin *Source, UEdGraphPin *Dest );
	
public:
	virtual void AllocateDefaultPins() override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool IsNodePure() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual FText GetMenuCategory() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

	virtual FText GetTooltipText() const override;
};
