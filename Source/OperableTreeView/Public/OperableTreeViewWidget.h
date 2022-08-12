// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TreeView.h"
#include "OperableTreeNode.h"
#include "OperableTreeViewWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTreeDataUpdate, TArray<FTreeData>,data);

UCLASS()
class OPERABLETREEVIEW_API UOperableTreeViewWidget : public UTreeView
{
	GENERATED_BODY()

public:
	UOperableTreeViewWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ListView)
		TSubclassOf<UObject> ObjectType;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Tree Data")
		UOperableTreeNode* Root;

	UPROPERTY(BlueprintAssignable, VisibleDefaultsOnly, Category = "Tree Data")
		FOnTreeDataUpdate OnTreeDataUpdate;

public:
	UFUNCTION(BlueprintCallable, Category = "Operable Tree | Init")
		void InitRoot(TArray<FTreeData> data);

	UFUNCTION(BlueprintCallable, Category = "Operable Tree | Tree")
		void UpdateTree();

	void StoreData();

protected:

private:
	void OnGetItemChildren(UObject* Item, TArray<UObject*>& Children);
	void GetNodeData(UOperableTreeNode* node);
	TArray<FTreeData> new_data;
};
	