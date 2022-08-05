// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "OperableTreeEntry.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EEntryDropZone : uint8
{
	None,
	AboverItem,
	OntoItem,
	BelowItem
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVisiblityChanged, bool, bVisible);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLockStateChanged, bool, bLocked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEntryDropZoneChanged, EEntryDropZone, DropZoneType);

class UDragItemVisual;
class UOperableTreeNode;
UCLASS()
class OPERABLETREEVIEW_API UOperableTreeEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	UOperableTreeEntry(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Entry Common")
		bool bVisible = true;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Entry Common")
		bool bLocked = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Entry Common")
		FString DisplayName = "";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Entry Common")
		TSubclassOf <UDragItemVisual> DragItemVisualClass;

	UPROPERTY(BlueprintAssignable, Category = "Callback")
		FOnVisiblityChanged OnEntryVisibilityChanged;

	UPROPERTY(BlueprintAssignable, Category = "Callback")
		FOnLockStateChanged OnEntryLockStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Callback")
		FOnEntryDropZoneChanged OnEntryDropZoneChanged;

public:
	UFUNCTION(BlueprintCallable, Category = "Operable Tree | Entry | Common")
		void ToggleVisiblity();

	UFUNCTION(BlueprintCallable, Category = "Operable Tree | Entry | Common")
		void ToggleLock();

	UFUNCTION(BlueprintNativeEvent, Category = "Operable Tree | Entry | Common")
		bool CanDragOnto();



protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);


	//drag
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation);
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);

	// user list entry interface override
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Entry Common", meta = (AllowPrivateAccess = "true"))
		UOperableTreeNode* CurrentNode;

	EEntryDropZone DropZoneType;

	void CalcNodeData(UOperableTreeNode* DropNode);
};
