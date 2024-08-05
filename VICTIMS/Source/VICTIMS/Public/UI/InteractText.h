
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractText.generated.h"

class UTextBlock;
UCLASS()
class VICTIMS_API UInteractText : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* InteractText;

	UFUNCTION()
	void ShowInteractText(const bool& bItem, const bool& bHousing, const bool& bHousingDel);

	UFUNCTION()
	void HideInteractText();
	
protected:
	UFUNCTION()
	virtual void NativeConstruct() override;
};
