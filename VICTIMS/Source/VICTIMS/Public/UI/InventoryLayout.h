
#pragma once

#include "CoreMinimal.h"
#include "WindowLayout.h"
#include "InventoryLayout.generated.h"

class UButton;
class UTextBlock;
class USlotLayout;
class UUniformGridPanel;
class UItemDragVisual;
UCLASS()
class VICTIMS_API UInventoryLayout : public UWindowLayout
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;

public:
	
	UPROPERTY()
	AVICTIMSPlayerController* pc;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UUniformGridPanel* InventoryGridPanel;
	
	UPROPERTY()
	TArray<USlotLayout*>InventorySlotsArray;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Gold_Amount;

	UPROPERTY(meta = (BindWidget))
	UButton* SellButton;

	void UpdateGoldAmount();

	UFUNCTION()
	virtual void OnClickSellButton();
};
