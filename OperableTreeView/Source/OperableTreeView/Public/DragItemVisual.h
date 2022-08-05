// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DragItemVisual.generated.h"

/**
 * 
 */
UCLASS()
class OPERABLETREEVIEW_API UDragItemVisual : public UUserWidget
{
	GENERATED_BODY()

public:
	UDragItemVisual(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Drag Visual")
		bool bEnable;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Drag Visual")
		FString DisplayName;

	
};
