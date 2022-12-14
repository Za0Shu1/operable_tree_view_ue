// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "EntryDragDropOperation.generated.h"

/**
 * 
 */

class UOperableTreeNode;
UCLASS()
class OPERABLETREEVIEW_API UEntryDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UEntryDragDropOperation();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Style")
		UOperableTreeNode* Node = nullptr;
	
};
