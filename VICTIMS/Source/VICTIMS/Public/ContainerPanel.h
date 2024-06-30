
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ContainerPanel.generated.h"

class UInventoryItemSlot;
class AVICTIMSPlayerController;
UCLASS()
class VICTIMS_API UContainerPanel : public UUserWidget
{
	GENERATED_BODY()

public: 

	void SetTextToContainer(ESlateVisibility inVisibility);

	virtual void OnButtonQuitClicked();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	class UUniformGridPanel* ContainerGridPanel;

	UPROPERTY()
	bool bIsStorageContainer;

	UPROPERTY()
	TArray<UInventoryItemSlot*> ContainerSlotsArray;

	UPROPERTY()
	AVICTIMSPlayerController* Controller;

private:

	virtual void NativeConstruct() override;
	
};
