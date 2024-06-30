
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HotbarPanel.generated.h"

class UHotbarItemSlot;
class UUniformGridPanel;
UCLASS()
class VICTIMS_API UHotbarPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UUniformGridPanel* HotbarGridPanel;

	UPROPERTY()
	TArray<UHotbarItemSlot*> HotbarSlotsArray;

};
