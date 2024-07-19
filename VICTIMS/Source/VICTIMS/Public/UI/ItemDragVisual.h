
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemDragVisual.generated.h"

class UTextBlock;
class UBorder;
class UImage;
UCLASS()
class VICTIMS_API UItemDragVisual : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Drag Item Visual", meta=(BindWidget))
	UImage* Icon;
	UPROPERTY(BlueprintReadOnly, Category="Drag Item Visual", meta=(BindWidget))
	UBorder* ItemBorder;
	UPROPERTY(BlueprintReadOnly, Category="Drag Item Visual", meta=(BindWidget))
	UTextBlock* Amount;

};
