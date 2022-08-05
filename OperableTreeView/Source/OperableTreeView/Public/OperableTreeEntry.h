// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "OperableTreeEntry.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVisiblityChanged, bool, bVisible);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLockStateChanged, bool, bLocked);

UCLASS()
class OPERABLETREEVIEW_API UOperableTreeEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	UOperableTreeEntry(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Entry Common")
		bool bVisible = true;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Entry Common")
		bool bLocked = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Entry Common")
		FString DisplayName = "";

	UPROPERTY(BlueprintAssignable, Category = "Callback")
		FOnVisiblityChanged OnEntryVisibilityChanged;

	UPROPERTY(BlueprintAssignable, Category = "Callback")
		FOnLockStateChanged OnEntryLockStateChanged;

public:
	UFUNCTION(BlueprintCallable, Category = "Operable Tree | Entry | Common")
		void ToggleVisiblity();

	UFUNCTION(BlueprintCallable, Category = "Operable Tree | Entry | Common")
		void ToggleLock();


protected:
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);


	//drag
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation);
// 	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);
// 	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);
// 	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);
// 	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);
// 	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);
};
