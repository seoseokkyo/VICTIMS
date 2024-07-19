
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FSlotStructure.h"
#include "ItemDragVisual.h"
#include "SlotLayout.generated.h"

class AVICTIMSPlayerController;
class UCanvasPanel;
class UTextBlock;
class UButton;
class UMenuAnchor;

UCLASS()
class VICTIMS_API USlotLayout : public UUserWidget
{
	GENERATED_BODY()


public:
	USlotLayout(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	uint8 InventorySlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	bool NativeFromInventory = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	bool NativeFromEquipment = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	bool NativeFromContainer = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default")
	bool NativeFromShop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlotStructure SlotStructure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ContainerSlots")
	bool IsStorageSlot = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShopSlots")
	bool IsShopSlot = false;
	
	UFUNCTION()
	void ToggleTooltip();

	UFUNCTION()
	void SetSlotIndex(uint8 Index);
	UFUNCTION()
	void SetSlotStructure(const FSlotStructure& SlotToSet);
	
	UFUNCTION()
	void UpdateSlot(const FSlotStructure& NewSlotStructure);
	UFUNCTION()
	void UpdateSlotInfo();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AVICTIMSPlayerController* PlayerController;

	void SetNameBoxVisibility();
	
	UFUNCTION()
	void UseItem();

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* Icon;
	
	UPROPERTY(meta = (BindWidget))
	UBorder* SlotBorder;

	UPROPERTY(meta = (BindWidget))
	UBorder* ItemBorder;

	UPROPERTY(meta = (BindWidget))
	UButton* SlotButton;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmountTextBlock;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* NameBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameText;

	UPROPERTY(meta = (BindWidget))
	UMenuAnchor* SlotMenuAnchor;

protected:
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

	UFUNCTION(Category="Default")
	FLinearColor GetBorderColor();
	UFUNCTION(Category="Default")
	void DisplayTooltip();
	UFUNCTION(Category="Default")
	void HideTooltip();
	UFUNCTION(Category="Default")
	bool HasItem();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UItemDragVisual> ItemDragVisualClass;


private:
	UFUNCTION()
	bool IsUnequipping(const uint8& LocalDraggedSlotIndex);
	UFUNCTION()
	bool IsEquipping(const uint8& InventorySlot);

	UPROPERTY()
	class UVictimsGameInstance* GameInstance;

	bool LeftMouseButtonClickedOnce;
};
