
#pragma once

#include "CoreMinimal.h"
#include "UI/SlotLayout.h"
#include "Blueprint/Userwidget.h"
#include "ItemDragVisual.h"
#include "FSlotSTructure.h"
#include "Shop_Slo.generated.h"

class AVICTIMSPlayerController;
class UCanvasPanel;
class UTextBlock;
class UButton;
class UMenuAnchor;

UCLASS()
class VICTIMS_API UShop_Slo : public USlotLayout
{
	GENERATED_BODY()


public:

 	UPROPERTY(meta=(BindWidget))
 	UTextBlock* PriceValue;
    UPROPERTY(meta=(BindWidget))
    class UBorder* NotificationBorder;

    UPROPERTY(meta=(BindWidget))
    class UTextBlock* NotificationText;
	
    UFUNCTION()
    void ShowNotification();
 
    UFUNCTION()
 	void SetPriceValue(const FItemStructure& Item);


 private:
 
    virtual void NativeConstruct() override;

};
