// Fill out your copyright notice in the Description page of Project Settings.


#include "OperableTreeEntry.h"
#include "EntryDragDropOperation.h"
#include "InputCoreTypes.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "DragItemVisual.h"

UOperableTreeEntry::UOperableTreeEntry(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bVisible = true;
	bLocked = false;
	DisplayName = "";
	DragItemVisualClass = UDragItemVisual::StaticClass();
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


bool UOperableTreeEntry::CanDragOnto_Implementation()
{
	return true;
}

FReply UOperableTreeEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply =  Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		Reply.DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	return Reply;
}

void UOperableTreeEntry::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UEntryDragDropOperation* temp = NewObject<UEntryDragDropOperation>();

	UDragItemVisual* DragVisual = CreateWidget<UDragItemVisual>(this, DragItemVisualClass);
	if (DragVisual)
	{
		DragVisual->bEnable = CanDragOnto();
		DragVisual->DisplayName = DisplayName;
	}

	// todo parse node data
	temp->Node = nullptr;
	temp->Payload = this;

	temp->DefaultDragVisual = DragVisual;
	temp->Pivot = EDragPivot::TopLeft;
	OutOperation = temp;
}

bool UOperableTreeEntry::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	OnEntryDropZoneChanged.Broadcast(EEntryDropZone::None);
	return true;
}

void UOperableTreeEntry::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry,InDragDropEvent, InOperation);
}

void UOperableTreeEntry::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	OnEntryDropZoneChanged.Broadcast(EEntryDropZone::None);
}

bool UOperableTreeEntry::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	const FVector2D LocalPointerPos = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	const FVector2D LocalSize = InGeometry.GetLocalSize();

	float ZoneBoundarySu = FMath::Clamp(LocalSize.Y * 0.25f, 3.f, 10.f);
	float PointPos = LocalPointerPos.Y;
	if (PointPos < ZoneBoundarySu)
	{
		OnEntryDropZoneChanged.Broadcast(EEntryDropZone::AboverItem);
	}
	else if (PointPos > LocalSize.Y - ZoneBoundarySu)
	{
		OnEntryDropZoneChanged.Broadcast(EEntryDropZone::BelowItem);
	}
	else
	{
		OnEntryDropZoneChanged.Broadcast(EEntryDropZone::OntoItem);
	}
	return Super::NativeOnDragOver(InGeometry,InDragDropEvent,InOperation);
}

void UOperableTreeEntry::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	OnEntryDropZoneChanged.Broadcast(EEntryDropZone::None);
}
