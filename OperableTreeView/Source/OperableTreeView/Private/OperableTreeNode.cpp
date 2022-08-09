// Fill out your copyright notice in the Description page of Project Settings.


#include "OperableTreeNode.h"

DEFINE_LOG_CATEGORY(LogTreeNode);

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
	UOperableTreeNode* _head = GetChildHead();
	
	if (_head)
	{
		ordered_leafs.Emplace(_head);
		UOperableTreeNode* _next = _head->next;

		// insert node by order
		while (_next)
		{
			ordered_leafs.Emplace(_next);
			_next = _next->next;
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

bool UOperableTreeNode::InsertChild(UOperableTreeNode* newNode, UOperableTreeNode* preNode)
{
	// invalid node
	if (!newNode) return false;
	// node already exist
	if (leafs.Contains(newNode)) return false;
	// preNode does not exist
	if (preNode && !leafs.Contains(preNode)) return false;
	PrintLeafs("Before Insert Child...");
	if (preNode)
	{
		newNode->SetNext(preNode->next);
		preNode->SetNext(newNode);
	}
	else
	{
		UOperableTreeNode* head = GetChildHead();
		newNode->SetNext(head);
	}
	leafs.Add(newNode);
	newNode->parent = this;
	PrintLeafs("After Insert Child...");
	return true;
}

bool UOperableTreeNode::RemoveChild(UOperableTreeNode* removeNode)
{
	// node does not exist
	if (!leafs.Contains(removeNode)) return false;
	PrintLeafs("Before Remove Child...");
	if (removeNode->GetPre())
	{
		removeNode->GetPre()->SetNext(removeNode->next);
	}
	leafs.Remove(removeNode);
	removeNode->parent = nullptr;
	PrintLeafs("After Remove Child...");
	return true;
}

bool UOperableTreeNode::AppendChild(UOperableTreeNode* newNode)
{
	// invalid node
	if (!newNode) return false;
	// node already exist
	if (leafs.Contains(newNode)) return false;
	PrintLeafs("Before Append Child...");
	UOperableTreeNode* node = GetChildHead();
	while (node)
	{
		if (node->next)
		{
			node = node->next;
		}
		else
		{
			node->next = newNode;
			leafs.Add(newNode);
			newNode->parent = this;
			PrintLeafs("After Append Child...");
			return true;
		}
	}
	return false;
}

UOperableTreeNode* UOperableTreeNode::GetChildHead()
{
	UOperableTreeNode* _head = nullptr;
	for (auto leaf : leafs)
	{
		if (leaf->GetPre() == nullptr)
		{
			_head = leaf;
			break;
		}
	}

	return _head;
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
	while (root->parent)
	{
		root = root->parent;
	}
	root->OnTreeNodeUpdate.Broadcast();
}

void UOperableTreeNode::PrintLeafs(FString progress)
{
	if (!progress.IsEmpty())
	{
		UE_LOG(LogTreeNode, Warning, TEXT("In Progress === %s"), *progress);
	}

	UOperableTreeNode* head = GetChildHead();
	UE_LOG(LogTreeNode, Display, TEXT("(%s) ====Print Child Leafs===="), parent ? *data.displayName : *FString("Root"));
	while (head)
	{
		UE_LOG(LogTreeNode, Display, TEXT("%s"),*(head->data.displayName));
		head = head->next;
	}
}

bool UOperableTreeNode::IsChildOf(UOperableTreeNode* compareNode)
{
	UOperableTreeNode* node = this;
	while (node && compareNode)
	{
		if (node->parent == compareNode)
		{
			return true;
		}
		node = node->parent;
	}
	return false;
}

