
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Hotbar.generated.h"

class UHotbar_Slot;
class UUniformGridPanel;
UCLASS()
class VICTIMS_API UHotbar : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* HotbarGridPanel;

	UPROPERTY()
	TArray<UHotbar_Slot*>HotbarSlotsArray;
};
