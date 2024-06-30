
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPanel.generated.h"

class UInventoryItemSlot;
class UInventoryComponent;
class AVICTIMSCharacter;
class UUniformGridPanel;
class UDragItemVisual;
class UTextBlock;

UCLASS()
class VICTIMS_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UUniformGridPanel* InventoryGridPanel;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<UInventoryItemSlot*> inventorySlotsArray;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Gold_Amount;

	void UpdateGoldAmount();
	
protected:

	virtual void NativeOnInitialized() override;
};
