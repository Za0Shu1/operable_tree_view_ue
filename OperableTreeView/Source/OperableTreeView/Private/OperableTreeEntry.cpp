// Fill out your copyright notice in the Description page of Project Settings.


#include "OperableTreeEntry.h"
#include "EntryDragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UOperableTreeEntry::UOperableTreeEntry(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bVisible = true;
	bLocked = false;
	DisplayName = "";
}

void UOperableTreeEntry::ToggleVisiblity()
{
	bVisible = !bVisible;
	OnEntryVisibilityChanged.Broadcast(bVisible);
}

void UOperableTreeEntry::ToggleLock()
{
	bLocked = !bLocked;
	OnEntryLockStateChanged.Broadcast(bLocked);
}


FReply UOperableTreeEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UOperableTreeEntry::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UEntryDragDropOperation* temp = NewObject<UEntryDragDropOperation>();
	temp->bEnableDrop = true;
	temp->DisplayName = DisplayName;
	OutOperation = temp;
}
