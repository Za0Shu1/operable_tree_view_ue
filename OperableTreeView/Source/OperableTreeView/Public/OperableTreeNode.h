// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OperableTreeNode.generated.h"

/**
 * 
 */

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
		UTexture2D* icon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OperableTreeView")
		FString displayName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OperableTreeView")
		TArray<int32> ChildIndex;// 孩子表示法

public:
	FTreeData():index(-1),level(0),icon(nullptr),displayName(""){}

	void PrintData()
	{
		FString childs = "";
		for (int32 child : ChildIndex)
		{
			childs += FString::FromInt(child) + ",";
		}
		UE_LOG(LogTemp,Display,TEXT("index:%d;level:%d;display name:%s;childs:%s"),index,level,*displayName,*childs)
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
		FTreeData GetData();

	UFUNCTION(BlueprintPure, Category = "Operable Tree | Tree Data")
		TArray<UOperableTreeNode*> GetLeafs();

	UFUNCTION(BlueprintPure, Category = "Operable Tree | Tree Data")
		UOperableTreeNode* GetParent();

	UFUNCTION(BlueprintPure, Category = "Operable Tree | Tree Data")
		UOperableTreeNode* GetPre();

	UFUNCTION(BlueprintPure, Category = "Operable Tree | Tree Data")
		UOperableTreeNode* GetNext();


	UFUNCTION(BlueprintPure, Category = "Operable Tree | Tree Data")
		bool CanExpand();

	void InitData(FTreeData td, TArray<FTreeData> tds);
	void SetNext(UOperableTreeNode* next_node);

private:
	TArray<UOperableTreeNode*> leafs;

	UOperableTreeNode* next = nullptr;
	UOperableTreeNode* parent = nullptr;
	FTreeData data;	
};
