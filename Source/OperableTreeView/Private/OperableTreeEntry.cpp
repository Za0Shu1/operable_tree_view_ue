// Fill out your copyright notice in the Description page of Project Settings.


#include "OperableTreeEntry.h"
#include "EntryDragDropOperation.h"
#include "InputCoreTypes.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "DragItemVisual.h"
#include "OperableTreeNode.h"
#include "OperableTreeViewWidget.h"

DEFINE_LOG_CATEGORY(LogTreeEntry);

UOperableTreeEntry::UOperableTreeEntry(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	DisplayName = "";
	DragItemVisualClass = UDragItemVisual::StaticClass();
	DropZoneType = EEntryDropZone::None;
}

void UOperableTreeEntry::ToggleVisiblity()
{
	bool bVisible = !CurrentNode->GetData().bVisible;
	CurrentNode->GetData().bVisible = bVisible;
	OnEntryVisibilityChanged.Broadcast(bVisible);
	StoreProperties();
}

void UOperableTreeEntry::ToggleLock()
{
	bool bLocked = !CurrentNode->GetData().bLocked;
	CurrentNode->GetData().bLocked = bLocked;
	OnEntryLockStateChanged.Broadcast(bLocked);
	UpdateChildsLockState(bLocked);
	StoreProperties();
}


void UOperableTreeEntry::UpdateChildsLockState(bool bLocked)
{
	for (auto node : CurrentNode->GetLeafs())
	{
		UOperableTreeViewWidget* tree = Cast<UOperableTreeViewWidget>(UUserListEntryLibrary::GetOwningListView(this));
		if (tree)
		{
			UOperableTreeEntry* entry = Cast<UOperableTreeEntry>(tree->GetEntryWidgetFromItem(node));
			if (entry)
			{
				entry->CurrentNode->GetData().bLocked = bLocked;
				entry->OnEntryLockStateChanged.Broadcast(bLocked);
				entry->UpdateChildsLockState(bLocked);
			}
		}
	}
}

void UOperableTreeEntry::ToggleExpand()
{
	UOperableTreeViewWidget* tree = Cast<UOperableTreeViewWidget>(UUserListEntryLibrary::GetOwningListView(this));
	if (tree)
	{
		tree->SetItemExpansion(CurrentNode, !CurrentNode->GetData().bIsExpanded);
	}
}

void UOperableTreeEntry::UpdateExpand(bool bExpanded)
{
	CurrentNode->GetData().bIsExpanded = bExpanded;
	StoreProperties();
}

bool UOperableTreeEntry::CanDragOnto_Implementation()
{
	return false;
}

bool UOperableTreeEntry::CanExpand()
{
	if (CurrentNode)
	{
		return CurrentNode->GetLeafs().Num() > 0;
	}
	return false;
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
		DragVisual->DisplayText = DisplayName;
		DragVisual->ItemType = CurrentNode->GetData().ItemType;
	}

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
		if (DropNode)
		{
			if (DropNode && CurrentNode)
			{
				// move to self is meaningless
				if (DropNode != CurrentNode)
				{
					if (DropZoneType != EEntryDropZone::None)
					{
						// means can drop
						UpdateNodeData(DropNode);
						UpdateDropZone(EEntryDropZone::None);
						return true;
					}
					else
					{
						return false;
					}
				}
			}
		}
		else
		{
			if (DropZoneType != EEntryDropZone::None)
			{
				UDragItemVisual* DragVisual = Cast<UDragItemVisual>(DropItemOperation->DefaultDragVisual);
				if (DragVisual)
				{
					AddNodeData(DragVisual->ItemType);
					UpdateDropZone(EEntryDropZone::None);
					return true;
				}
			}
		}
	}
	UpdateDropZone(EEntryDropZone::None);
	return false;
}

void UOperableTreeEntry::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry,InDragDropEvent, InOperation);
}

void UOperableTreeEntry::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	UpdateDropZone(EEntryDropZone::None);
}

bool UOperableTreeEntry::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UEntryDragDropOperation* DropItemOperation = Cast<UEntryDragDropOperation>(InOperation);
	
	if (DropItemOperation)
	{
		UOperableTreeNode* DropNode = DropItemOperation->Node;
		if (DropNode)
		{
			// drag to self, meaningless
			if (DropNode == CurrentNode)
			{
				UDragItemVisual* DragVisual = Cast<UDragItemVisual>(DropItemOperation->DefaultDragVisual);
				if (DragVisual)
				{
					DragVisual->bEnable = false;
					DragVisual->DisplayText = DisplayName;
					DragVisual->OnStateChanged.Broadcast();
				}
				UpdateDropZone(EEntryDropZone::None);
				return Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);;
			}

			// drag to it's child,disable operation
			else if (CurrentNode->IsChildOf(DropNode))
			{
				UDragItemVisual* DragVisual = Cast<UDragItemVisual>(DropItemOperation->DefaultDragVisual);
				if (DragVisual)
				{
					DragVisual->bEnable = false;
					DragVisual->DisplayText = FString("can't make widget a child of its children");
					DragVisual->OnStateChanged.Broadcast();
				}
				UpdateDropZone(EEntryDropZone::None);
				return Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);;
			}
		}
	}

	// calc mouse location in this item
	const FVector2D LocalPointerPos = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	const FVector2D LocalSize = InGeometry.GetLocalSize();

	float ZoneBoundarySu = FMath::Clamp(LocalSize.Y * 0.25f, 3.f, 10.f);
	float PointPos = LocalPointerPos.Y;
	if (PointPos < ZoneBoundarySu)
	{
		// can always move above
		UDragItemVisual* DragVisual = Cast<UDragItemVisual>(DropItemOperation->DefaultDragVisual);
		if (DragVisual)
		{
			DragVisual->bEnable = true;
			DragVisual->DisplayText = DisplayName;
			DragVisual->OnStateChanged.Broadcast();
		}

		UpdateDropZone(EEntryDropZone::AboverItem);
	}
	else if (PointPos > LocalSize.Y - ZoneBoundarySu)
	{
		// can always move below
		UDragItemVisual* DragVisual = Cast<UDragItemVisual>(DropItemOperation->DefaultDragVisual);
		if (DragVisual)
		{
			DragVisual->bEnable = true;
			DragVisual->DisplayText = DisplayName;
			DragVisual->OnStateChanged.Broadcast();
		}

		UpdateDropZone(EEntryDropZone::BelowItem);
	}
	else
	{
		// calc if this item can contains children
		UDragItemVisual* DragVisual = Cast<UDragItemVisual>(DropItemOperation->DefaultDragVisual);
		if (DragVisual)
		{
			DragVisual->bEnable = CanDragOnto();
			DragVisual->DisplayText = CanDragOnto() ? DisplayName : FString("widget can't accept additional children");
			DragVisual->OnStateChanged.Broadcast();
			UpdateDropZone(CanDragOnto() ? EEntryDropZone::OntoItem : EEntryDropZone::None);
		}
		else
		{
			UpdateDropZone(EEntryDropZone::None);
		}
	}
	return Super::NativeOnDragOver(InGeometry,InDragDropEvent,InOperation);
}

FReply UOperableTreeEntry::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Delete)
	{
		UOperableTreeNode* Parent = CurrentNode->GetParent();
		if (Parent)
		{
			if (Parent->RemoveChild(CurrentNode))
			{
				Parent->UpdateTree();
			}
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UOperableTreeEntry::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	UpdateDropZone(EEntryDropZone::None);
}

void UOperableTreeEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (!ListItemObject) return;
	CurrentNode = Cast<UOperableTreeNode>(ListItemObject);
	if (CurrentNode)
	{
		DisplayName = CurrentNode->GetData().displayName;
	}
	Execute_OnListItemObjectSet(Cast<UObject>(this), ListItemObject);
}

void UOperableTreeEntry::UpdateNodeData(UOperableTreeNode* DropNode)
{
	if (!DropNode) return;

	switch (DropZoneType)
	{
		case EEntryDropZone::None:
			break;
		case EEntryDropZone::AboverItem:
		{
			// order does not change
			if (DropNode->GetNext() == CurrentNode) return;

			UOperableTreeNode* dropParent = DropNode->GetParent();
			if (dropParent)
			{
				if (dropParent->RemoveChild(DropNode)) 
				{
					UOperableTreeNode* curParent = CurrentNode->GetParent();
					if (curParent)
					{
						if (curParent->InsertChild(DropNode, CurrentNode->GetPre()))
						{
							DropNode->UpdateTree();
						}
						return;
					}
				}
			}
			break;
		}
		case EEntryDropZone::OntoItem:
		{
			// already is its children
			if (DropNode->GetParent() == CurrentNode)  return;

			UOperableTreeNode* dropParent = DropNode->GetParent();
			if (dropParent)
			{
				if (dropParent->RemoveChild(DropNode))
				{
					if (CurrentNode->AppendChild(DropNode))
					{
						DropNode->UpdateTree();
					}
					return;
				}
			}
			break;
		}
		case EEntryDropZone::BelowItem:
		{
			// order does not change
			if (CurrentNode->GetNext() == DropNode)  return;
			
			UOperableTreeNode* dropParent = DropNode->GetParent();
			if (dropParent)
			{
				if (dropParent->RemoveChild(DropNode))
				{
					UOperableTreeNode* curParent = CurrentNode->GetParent();
					if (curParent)
					{
						if (curParent->InsertChild(DropNode, CurrentNode))
						{
							DropNode->UpdateTree();
						}
						return;
					}
				}
			}
			break;
		}
			
		default:
			break;
	}
}

void UOperableTreeEntry::AddNodeData(EItemType NewItemType)
{
	UOperableTreeViewWidget* tree = Cast<UOperableTreeViewWidget>(UUserListEntryLibrary::GetOwningListView(this));
	if (!tree) return;

	// get new data
	int32 index = -1;
	FString displayName = "None";
	tree->AssignItemInfo(index, displayName, NewItemType);

	// create node
	UOperableTreeNode* newNode = NewObject<UOperableTreeNode>();
	FTreeData _data;
	_data.index = index;
	_data.displayName = displayName;
	_data.ItemType = NewItemType;
	newNode->InitData(_data, {});

	// insert data
	switch (DropZoneType)
	{
		case EEntryDropZone::None:
			break;
		case EEntryDropZone::AboverItem:
			{
				UOperableTreeNode* curParent = CurrentNode->GetParent();
				if (curParent)
				{
					if (curParent->InsertChild(newNode, CurrentNode->GetPre()))
					{
						newNode->UpdateTree();
					}
					return;
				}
				break;
			}
		case EEntryDropZone::OntoItem:
		{
			if (CurrentNode->AppendChild(newNode))
			{
				newNode->UpdateTree();
				return;
			}
			break;
		}
		case EEntryDropZone::BelowItem:
		{
			UOperableTreeNode* curParent = CurrentNode->GetParent();
			if (curParent)
			{
				if (curParent->InsertChild(newNode, CurrentNode))
				{
					newNode->UpdateTree();
				}
				return;
			}
			break;
		}
		default:
			break;
	}
}

void UOperableTreeEntry::StoreProperties()
{
	UOperableTreeViewWidget* tree = Cast<UOperableTreeViewWidget>(UUserListEntryLibrary::GetOwningListView(this));
	if (tree)
	{
		tree->StoreData();
	}
}
	
