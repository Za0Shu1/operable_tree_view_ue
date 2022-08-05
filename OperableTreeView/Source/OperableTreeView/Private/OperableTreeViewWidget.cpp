// Fill out your copyright notice in the Description page of Project Settings.


#include "OperableTreeViewWidget.h"

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
	FTreeData root_data;
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

void UOperableTreeViewWidget::OnGetItemChildren(UObject* Item,TArray<UObject*>& Children)
{
	UOperableTreeNode* obj = Cast<UOperableTreeNode>(Item);
	if (obj)
	{
		for (auto o : obj->GetLeafs())
		{
			Children.Add(Cast<UObject>(o));
		}
	}
}
