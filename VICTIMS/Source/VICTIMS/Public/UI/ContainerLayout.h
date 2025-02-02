
#pragma once

#include "CoreMinimal.h"
#include "WindowLayout.h"
#include "Blueprint/UserWidget.h"
#include "ContainerLayout.generated.h"

class USlotLayout;
UCLASS()
class VICTIMS_API UContainerLayout : public UWindowLayout
{
	GENERATED_BODY()

public:	
	void SetTextToContainer(ESlateVisibility InVisibility);

	virtual void OnButtonQuitClicked() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UUniformGridPanel* ContainerGridPanel;
	
	UPROPERTY()
	bool IsStorageContainer;
	
	UPROPERTY()
	TArray<USlotLayout*>ContainerSlotsArray;

private:
	virtual void NativeConstruct() override;
};
