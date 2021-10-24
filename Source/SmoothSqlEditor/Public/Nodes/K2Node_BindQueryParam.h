// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_BindQueryParam.generated.h"

/**
 * Wrapper over binding to sql statements
 */
UCLASS()
class SMOOTHSQLEDITOR_API UK2Node_BindQueryParam : public UK2Node
{
	GENERATED_BODY()

	UEdGraphPin* GetValuePin() const;
	UEdGraphPin* GetParamPin() const;
	UEdGraphPin* GetTargetPin() const;

	void MovePinLinksOrCopyDefaults( FKismetCompilerContext &CompilerContext, UEdGraphPin *Source, UEdGraphPin *Dest );
	
public:
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool IsNodePure() const override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetKeywords() const override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	void SyncParamType(UEdGraphPin* Pin);
	virtual void ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const override;
	virtual void PinTypeChanged(UEdGraphPin* Pin) override;
	virtual void ReconstructNode() override;
	void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
};
