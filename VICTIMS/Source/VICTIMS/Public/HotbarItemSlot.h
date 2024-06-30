
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FSlotStructure.h"
#include "HotbarItemSlot.generated.h"

class UBorder;
class UButton;
class UImage;
class UTextBlock;
class AVICTIMSPlayerController;
UCLASS()
class VICTIMS_API UHotbarItemSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	uint8 HotbarSlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool NativeFromHotbar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AVICTIMSPlayerController* PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlotStructure SlotStructure;

	UFUNCTION()
	void UseItem();


	UFUNCTION()
	void UpdateSlot(const FSlotStructure& NewSlotStructure);
	UFUNCTION()
	void SetSlotStructure(const FSlotStructure& SlotToSet);
	UFUNCTION()
	void UpdateSlotInfo();

	UFUNCTION()
	void SetSlotIndex(uint8 Index);

	UFUNCTION()
	void SetKeyNumber(uint8 InNumber);

protected:
	UPROPERTY(meta = (BindWidget))
	UBorder* SlotBorder;

	UPROPERTY(meta = (BindWidget))
	UButton* SlotButton;

	UPROPERTY(meta = (BindWidget))
	UImage* Icon;

	UPROPERTY(meta = (BindWidget))
	UBorder* ItemBorder;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Number;

	UPROPERTY(EditDefaultsOnly)
	UClass* ItemDragVisualClass;

	UFUNCTION(Category = "Default")
	bool HasItem();




protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	FReply CustomDetectDrag(const FPointerEvent& InMouseEvent, UWidget* WidgetDetectingDrag, FKey DragKey);

private:
	bool LeftMouseButtonClickedOnce;
};
