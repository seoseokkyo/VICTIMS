
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProfilePanel.generated.h"

class UTextBlock;
class UUniformGridPanel;
class UInventoryItemSlot;
UCLASS()
class VICTIMS_API UProfilePanel : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UUniformGridPanel* EquipmentGridpanel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* attributes;

	UPROPERTY()
	TArray<UInventoryItemSlot*> EquipmentSlotArray;

private:

	virtual void NativeConstruct() override;
};
