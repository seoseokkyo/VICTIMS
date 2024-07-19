
#pragma once

#include "CoreMinimal.h"
#include "UI/WindowLayout.h"
#include "DropMoneyLayout.generated.h"


UCLASS()
class VICTIMS_API UDropMoneyLayout : public UWindowLayout
{
	GENERATED_BODY()

public:

	UPROPERTY(meta=(BindWidget))
	class UEditableText* Amount;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* DropButton;

	UPROPERTY(meta=(BindWidget))
	class UBorder* NotificationBorder;

	UFUNCTION()
	void OnClickedDropButton();

	virtual void NativeConstruct() override;
};
