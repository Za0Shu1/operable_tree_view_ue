// Fill out your copyright notice in the Description page of Project Settings.


#include "OperableTreeEntry.h"
#include "EntryDragDropOperation.h"
#include "InputCoreTypes.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "DragItemVisual.h"
#include "OperableTreeNode.h"

UOperableTreeEntry::UOperableTreeEntry(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bVisible = true;
	bLocked = false;
	DisplayName = "";
	DragItemVisualClass = UDragItemVisual::StaticClass();
	DropZoneType = EEntryDropZone::None;
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
	temp->Payload = nullptr;
	temp->Node = CurrentNode;

	temp->DefaultDragVisual = DragVisual;
	temp->Pivot = EDragPivot::TopLeft;
	OutOperation = temp;
}

bool UOperableTreeEntry::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UEntryDragDropOperation* DropItemOperation = Cast<UEntryDragDropOperation>(InOperation);
	if (DropItemOperation)
	{
		UOperableTreeNode* DropNode = DropItemOperation->Node;
		if (DropNode && CurrentNode)
		{
			// move to self is meaningless
			if (DropNode != CurrentNode)
			{
				CalcNodeData(DropNode);
				DropZoneType = EEntryDropZone::None;
				OnEntryDropZoneChanged.Broadcast(EEntryDropZone::None);
				return true;
			}
		}
	}
	DropZoneType = EEntryDropZone::None;
	OnEntryDropZoneChanged.Broadcast(EEntryDropZone::None);
	return false;
}

void UOperableTreeEntry::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry,InDragDropEvent, InOperation);
}

void UOperableTreeEntry::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	DropZoneType = EEntryDropZone::None;
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
		DropZoneType = EEntryDropZone::AboverItem;
		OnEntryDropZoneChanged.Broadcast(EEntryDropZone::AboverItem);
	}
	else if (PointPos > LocalSize.Y - ZoneBoundarySu)
	{
		DropZoneType = EEntryDropZone::BelowItem;
		OnEntryDropZoneChanged.Broadcast(EEntryDropZone::BelowItem);
	}
	else
	{
		DropZoneType = EEntryDropZone::OntoItem;
		OnEntryDropZoneChanged.Broadcast(EEntryDropZone::OntoItem);
	}
	return Super::NativeOnDragOver(InGeometry,InDragDropEvent,InOperation);
}

void UOperableTreeEntry::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	OnEntryDropZoneChanged.Broadcast(EEntryDropZone::None);
	DropZoneType = EEntryDropZone::None;
}

void UOperableTreeEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (!ListItemObject) return;
	CurrentNode = Cast<UOperableTreeNode>(ListItemObject);
	Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}

void UOperableTreeEntry::CalcNodeData(UOperableTreeNode* DropNode)
{
	if (!DropNode) return;

	// have same parent
	if (DropNode->GetParent() == CurrentNode->GetParent())
	{
		switch (DropZoneType)
		{
		case EEntryDropZone::None:
			break;
		case EEntryDropZone::AboverItem:
			{
				if (DropNode->GetNext() == CurrentNode) return;// order does not change

				//Current Item
				UOperableTreeNode* self_pre = CurrentNode->GetPre();
				UOperableTreeNode* self_next = CurrentNode->GetNext();

				//Drop Item
				UOperableTreeNode* drop_pre = DropNode->GetPre();
				UOperableTreeNode* drop_next = DropNode->GetNext();

				DropNode->SetNext(CurrentNode);
				CurrentNode->SetNext(drop_next);
				if (self_pre)
				{
					self_pre->SetNext(DropNode);
				}
			}
			break;
		case EEntryDropZone::OntoItem:
			break;
		case EEntryDropZone::BelowItem:
			if (CurrentNode->GetNext() == DropNode) return; // order does not change

			break;
		default:
			break;
		}
	}

		
	
}
