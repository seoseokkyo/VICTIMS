
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EAttributes.h"
#include "FSlotStructure.h"
#include "FItemStructure.h"
#include "W_ItemTooltip.generated.h"

class UImage;
class UTextBlock;
class UUniformGridPanel;
class UVerticalBox;

UCLASS()
class VICTIMS_API UW_ItemTooltip : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FSlotStructure EquippedSlotOnProfile;

	UFUNCTION()
	void UpdateTooltipAttributes(const FItemStructure& Item, const FSlotStructure& EquippedSlot);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Index;
	
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Name;

	UPROPERTY(meta = (BindWidget))
	UImage* Icon;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Description;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Type;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageValue;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealValue;

	UPROPERTY(meta  = (BindWidget))
	UTextBlock* PriceValue;


private:
    virtual void NativeConstruct() override;

	void SetDescription(const FItemStructure& Item);
	void SetItemType(const FItemStructure& Item);
	void SetItemName(const FItemStructure& Item);
	void SetAttributes(const FItemStructure& Item);

	UFUNCTION()
	void GetAttributeValueFromItem(const FItemStructure& Item, EAttributes Attribute, int8& Value);

	UPROPERTY()
	class UVictimsGameInstance* GameInstance;
};
