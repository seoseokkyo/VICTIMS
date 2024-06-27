
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DragItemVisual.generated.h"

class UBorder;
class UTextBlock;
class UImage;

UCLASS()
class VICTIMS_API UDragItemVisual : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, Category="Drag Item Visual", meta=(BindWidget))
	UBorder* ItemBorder;

	UPROPERTY(VisibleAnywhere, Category="Drag Item Visual", meta=(BindWidget))
	UImage* ItemIcon;

	UPROPERTY(VisibleAnywhere, Category="Drag Item Visual", meta=(BindWidget))
	UTextBlock* ItemQuantity;
};
