// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OperableTreeNode.h"
#include "DragItemVisual.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStateChanged);

UCLASS()
class OPERABLETREEVIEW_API UDragItemVisual : public UUserWidget
{
	GENERATED_BODY()

public:
	UDragItemVisual(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Drag Visual" , meta=(ExposeOnSpawn = true))
		bool bEnable;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Drag Visual" , meta = (ExposeOnSpawn = true))
		FString DisplayText;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Drag Visual", meta = (ExposeOnSpawn = true))
		EItemType ItemType;

	UPROPERTY(BlueprintAssignable, Category = "Drag Visual")
		FOnStateChanged OnStateChanged;
};
