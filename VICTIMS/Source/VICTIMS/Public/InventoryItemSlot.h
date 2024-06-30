
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FSlotStructure.h"
#include "InventoryItemSlot.generated.h"

class AVICTIMSPlayerController;
class UCanvasPanel;
class UInventoryComponent;
class UInventoryTooltip;
class UDragItemVisual;
class UItemBase;
class UTextBlock;
class UBorder;
class UImage;
class UMenuAnchor;

UCLASS()
class VICTIMS_API UInventoryItemSlot : public UUserWidget
{
	GENERATED_BODY()


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	uint8 InventorySlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool NativeFromInventory = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool NativeFromEquipment = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool NativeFromContainer = false;

	UFUNCTION()
	void ToggleTooltip();

	UFUNCTION()
	void OpenSlotMenu();
	UFUNCTION()
	void CloseSlotMenu();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlotStructure SlotStructure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ContainerSlots")
	bool IsStorageSlot = false;

	UFUNCTION()
	void SetSlotIndex(uint8 Index);
	UFUNCTION()
	void SetSlotStructure(const FSlotStructure& SlotToSet);

	UFUNCTION()
	void UpdateSlot(const FSlotStructure& NewSlotStructure);
	UFUNCTION()
	void UpdateSlotInfo();

	UFUNCTION()
	void UseItem();

	UPROPERTY(VisibleAnywhere, Category="Inventory Slot")
	AVICTIMSPlayerController* playerController;

	UPROPERTY(meta=(BindWidget))
	UImage* icon;

	UPROPERTY(meta=(BindWidget))
	UBorder* slotBorder;

	UPROPERTY(meta=(BindWidget))
	UButton* slotButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* amountTextBlock;

	UPROPERTY(meta = (BindWidget))
	UMenuAnchor* slotMenuAnchor;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDragItemVisual> itemDragVisualClass;

	UFUNCTION()
	bool IsUnequipping(const uint8& LocalDraggedSlotIndex);
	UFUNCTION()
	bool IsEquipping(const uint8& InventorySlot);
	UFUNCTION(Category = "Default")
	bool HasItem();
	UFUNCTION(Category = "Default")
	FLinearColor GetBorderColor();
	UFUNCTION(Category = "Default")
	void DisplayTooltip();
	UFUNCTION(Category = "Default")
	void HideTooltip();

	bool LeftMouseButtonClickedOnce;

	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	FReply CustomDetectDrag(const FPointerEvent& InMouseEvent, UWidget* WidgetDetectingDrag, FKey DragKey);
};
