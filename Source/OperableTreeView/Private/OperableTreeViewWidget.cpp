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
			Children.Add(Cast<UObject>(o));
		}
	}
}

void UOperableTreeViewWidget::StoreData()
{
	new_data.Empty();
}

