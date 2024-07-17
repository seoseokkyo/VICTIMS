
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TestIDWidget.generated.h"


UCLASS()
class VICTIMS_API UTestIDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UEditableText* ID;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UButton* SignInButton;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UButton* ContinueButton; 

	UFUNCTION()
	void OnClickedSignInButton();
	UFUNCTION()
	void OnClickedContinueButton();

	UPROPERTY()
	bool IsIDValid = false;

protected:

	virtual void NativeConstruct() override;
};
