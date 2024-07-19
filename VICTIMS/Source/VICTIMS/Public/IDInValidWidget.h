
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IDInValidWidget.generated.h"


UCLASS()
class VICTIMS_API UIDInValidWidget : public UUserWidget
{
	GENERATED_BODY()

public:

// 	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* ValidInformText;
	
};
