// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OperableTreeNode.generated.h"

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(LogTreeNode, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTreeNodeUpdate);


UENUM(BlueprintType)
enum class EItemType : uint8
{
	None,
	CanvasPanel,
	Border,
	Button,
	CheckBox,
	Image,
	TextBlock,
	UserWidget
};

USTRUCT(BlueprintType)
struct FTreeData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OperableTreeView")
		int32 index;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OperableTreeView")
		int32 level;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OperableTreeView")
		EItemType ItemType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OperableTreeView")
		FString displayName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OperableTreeView")
		TArray<int32> ChildIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OperableTreeView")
		bool bIsExpanded;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OperableTreeView")
		bool bVisible;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OperableTreeView")
		bool bLocked;

public:
	FTreeData():index(-1),level(-1), ItemType(EItemType::None),displayName(""),
		ChildIndex({}), bIsExpanded(true), bVisible(true), bLocked(false) {}

	void PrintData()
	{
		if (index < 0) return;
		FString childs = "";
		for (int32 child : ChildIndex)
		{
			childs += FString::FromInt(child) + ",";
		}
		childs.RemoveFromEnd(",");
		UE_LOG(LogTreeNode, Display, TEXT("Name(%s) , Level(%d) , Child(%s)."), *displayName, level,*childs);
	}

};

UCLASS(Blueprintable)
class OPERABLETREEVIEW_API UOperableTreeNode : public UObject
{
	GENERATED_BODY()
	
public:
	UOperableTreeNode();

public:
	UFUNCTION(BlueprintPure, Category = "Operable Tree | Tree Data")
		FTreeData& GetData();

	UFUNCTION(BlueprintPure, Category = "Operable Tree | Tree Data")
		TArray<UOperableTreeNode*> GetLeafs();

	UFUNCTION(BlueprintPure, Category = "Operable Tree | Tree Data")
		UOperableTreeNode* GetParent();

	UFUNCTION(BlueprintPure, Category = "Operable Tree | Tree Data")
		UOperableTreeNode* GetPre();

	UFUNCTION(BlueprintPure, Category = "Operable Tree | Tree Data")
		UOperableTreeNode* GetNext();

	UPROPERTY(BlueprintAssignable,Category = "Callback")
		FOnTreeNodeUpdate OnTreeNodeUpdate;

	bool InsertChild(UOperableTreeNode* newNode, UOperableTreeNode* preNode);
	bool RemoveChild(UOperableTreeNode* removeNode);
	bool AppendChild(UOperableTreeNode* newNode);
	UOperableTreeNode* GetChildHead();
	void InitData(FTreeData td, TArray<FTreeData> tds);
	void SetNext(UOperableTreeNode* next_node);
	void UpdateNodeLevel(int32 new_level);
	void UpdateTree();
	void PrintLeafs(FString progress);
	void PrintTree();
	bool IsChildOf(UOperableTreeNode* compareNode);
private:
	TArray<UOperableTreeNode*> leafs;

	UOperableTreeNode* next = nullptr;
	UOperableTreeNode* parent = nullptr;
	FTreeData data;	
};
