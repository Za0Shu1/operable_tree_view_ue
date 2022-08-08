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
	TArray<UOperableTreeNode*> ordered_leafs;

	// get head node
	UOperableTreeNode* _head = nullptr;
	for (auto leaf : leafs)
	{
		if (leaf->GetPre() == nullptr)
		{
			_head = leaf;
			break;
		}
	}
	if (_head)
	{
		ordered_leafs.Emplace(_head);
		UOperableTreeNode* _next = _head->GetNext();

		// insert node by order
		while (_next)
		{
			ordered_leafs.Emplace(_next);
			_next = _next->GetNext();
		}
	}
	
	return ordered_leafs;
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

void UOperableTreeNode::UpdateTree()
{
	UOperableTreeNode* root = this;
	while (root->GetParent())
	{
		root = root->GetParent();
	}
	root->OnTreeNodeUpdate.Broadcast();
}

