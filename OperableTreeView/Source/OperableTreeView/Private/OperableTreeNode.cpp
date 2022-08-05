// Fill out your copyright notice in the Description page of Project Settings.


#include "OperableTreeNode.h"

UOperableTreeNode::UOperableTreeNode()
{
	
}

FTreeData UOperableTreeNode::GetData()
{
	return data;
}

TArray<UOperableTreeNode*> UOperableTreeNode::GetLeafs()
{
	return leafs;
}

UOperableTreeNode* UOperableTreeNode::GetParent()
{
	return parent;
}

UOperableTreeNode* UOperableTreeNode::GetPre()
{
	if (parent)
	{
		for (auto leaf : parent->leafs)
		{
			if (leaf->next == this)
			{
				return leaf;
			}
		}
	}
	return nullptr;
}

UOperableTreeNode* UOperableTreeNode::GetNext()
{
	return next;
}

bool UOperableTreeNode::CanExpand()
{
	return leafs.Num() > 0;
}

// Init Node Data and leafs
void UOperableTreeNode::InitData(FTreeData td, TArray<FTreeData> tds)
{
	this->data = td;

	if (td.ChildIndex.Num() == 0)
	{
		return;
	}
	else
	{
		// set leafs and parent
		for (int32 child_index : data.ChildIndex)
		{
			for (FTreeData _td : tds)
			{
				if (child_index == _td.index)
				{
					UOperableTreeNode* leaf = NewObject<UOperableTreeNode>();
					leaf->InitData(_td, tds);
					leaf->parent = this;
					leafs.Add(leaf);
					break;
				}
			}
		}

		// init leafs order
		if (leafs.Num() > 0)
		{
			int32 index = 0;
			while (leafs.IsValidIndex(index+1))
			{
				leafs[index]->next = leafs[index + 1];
				++index;
			}
		}
	}
}

void UOperableTreeNode::SetNext(UOperableTreeNode* next_node)
{
	next = next_node;
}

