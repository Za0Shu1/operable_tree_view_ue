// Fill out your copyright notice in the Description page of Project Settings.


#include "OperableTreeViewWidget.h"
#include "OperableTreeEntry.h"

UOperableTreeViewWidget::UOperableTreeViewWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->EntryWidgetClass = nullptr;
	this->EntrySpacing = 0.f;
	Root = nullptr;
	this->ObjectType = UOperableTreeNode::StaticClass();// default object
	this->SetOnGetItemChildren(this, &UOperableTreeViewWidget::OnGetItemChildren);// get children
}

// Init Root Tree
void UOperableTreeViewWidget::InitRoot(TArray<FTreeData> data)
{
	stored_data = data;
	// Create another tree to store all trees
	Root = NewObject<UOperableTreeNode>(this);
	Root->OnTreeNodeUpdate.AddDynamic(this, &UOperableTreeViewWidget::UpdateTree);
	FTreeData root_data;
	root_data.index = -1;
	root_data.level = -1;
	for (FTreeData td : data)
	{
		// trees
		if (td.level == 0)
		{
			root_data.ChildIndex.Add(td.index);
		}
	}
	Root->InitData(root_data, data);
	for (UOperableTreeNode* leaf : Root->GetLeafs())
	{
		this->AddItem(leaf);
		this->SetItemExpansion(leaf, leaf->GetData().bIsExpanded);
	}
}

void UOperableTreeViewWidget::UpdateTree()
{
	this->ClearListItems();
	for (UOperableTreeNode* leaf : Root->GetLeafs())
	{
		this->AddItem(leaf);
	}
	
	this->SetOnGetItemChildren(this, &UOperableTreeViewWidget::OnGetItemChildren);

	for (auto a : GetListItems())
	{
		UOperableTreeEntry* entry = Cast<UOperableTreeEntry>(GetEntryWidgetFromItem(a));
		if (entry)
		{
			entry->UpdateEntry();
		}
	}
	StoreData();
}

void UOperableTreeViewWidget::OnGetItemChildren(UObject* Item,TArray<UObject*>& Children)
{
	UOperableTreeNode* obj = Cast<UOperableTreeNode>(Item);
	if (obj)
	{
		for (auto o : obj->GetLeafs())
		{
			UOperableTreeEntry* entry = Cast<UOperableTreeEntry>(GetEntryWidgetFromItem(o));
			if (entry)
			{
				entry->UpdateEntry();
			}
			this->SetItemExpansion(o, o->GetData().bIsExpanded);
			Children.Add(Cast<UObject>(o));
		}
	}
}

void UOperableTreeViewWidget::StoreData()
{
	stored_data.Empty();
	for (auto leaf : Root->GetLeafs())
	{
		GetNodeData(leaf);
	}
	OnTreeDataUpdate.Broadcast(stored_data);
}

void UOperableTreeViewWidget::AssignItemInfo(int32& Index, FString& DisplayName, EItemType NewItemType)
{
	int32 maxIndex = -1;
	for (auto data : stored_data)
	{
		maxIndex = FMath::Max(maxIndex, data.index);
	}
	Index = maxIndex + 1;
}

void UOperableTreeViewWidget::GetNodeData(UOperableTreeNode* node)
{
	FTreeData curData;
	curData = node->GetData();

	// get childs data
	TArray<int32> childs;
	for (auto leaf : node->GetLeafs())
	{
		childs.Emplace(leaf->GetData().index);
	}
	curData.ChildIndex = childs;

	// add current leaf data
	stored_data.Add(curData);

	for (auto leaf : node->GetLeafs())
	{
		GetNodeData(leaf);
	}
}

