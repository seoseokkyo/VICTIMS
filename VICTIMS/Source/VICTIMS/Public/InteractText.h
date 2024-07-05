
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
	
protected:
	UFUNCTION()
	virtual void NativeConstruct() override;
};
