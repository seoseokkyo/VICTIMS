
#pragma once

#include "CoreMinimal.h"
#include "UI/WindowLayout.h"
#include "Blueprint/UserWidget.h"
#include "EAttributes.h"
#include "FSlotStructure.h"
#include "FItemStructure.h"
#include "ShopLayout.generated.h"

class USlotLayout;
class UShop_Slo;
class UTextBlock;
class UVerticalBox;
class UScrollBox;
UCLASS()
class VICTIMS_API UShopLayout : public UWindowLayout
{
	GENERATED_BODY()
private:
	virtual void NativeConstruct() override;
public:
	
// 	UPROPERTY(EditAnywhere, Category = "Notification", meta=(BindWidgetAnim), Transient)
// 	class UWidgetAnimation* Notification;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	class UScrollBox* ShopGirdPanel;
	UPROPERTY()
	bool IsShop;
	UPROPERTY()
	TArray<UShop_Slo*>ShopSlotsArray;

};
