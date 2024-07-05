
#pragma once

#include "CoreMinimal.h"
#include "WindowLayout.h"
#include "InventoryLayout.generated.h"

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UUniformGridPanel* InventoryGridPanel;
	
	UPROPERTY()
	TArray<USlotLayout*>InventorySlotsArray;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Gold_Amount;

	void UpdateGoldAmount();
};
