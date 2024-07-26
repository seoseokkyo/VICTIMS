
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TestIDWidget.generated.h"


UCLASS()
class VICTIMS_API UTestIDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))	// �Է¹޴� ���̵� (string)
	class UEditableText* ID;
	UPROPERTY(EditAnywhere, meta=(BindWidget))	// �Է¹޴� ��й�ȣ (string)
	class UEditableText* PassWord;
	UPROPERTY(EditAnywhere, meta=(BindWidget))	// register 
	class UButton* SignInButton;
	UPROPERTY(EditAnywhere, meta=(BindWidget))	// login
	class UButton* ContinueButton; 
	UPROPERTY(EditAnywhere, meta=(BindWidget))	// quit 
	class UButton* QuitButton;
	UPROPERTY(EditAnywhere, Category = "Sound")
	class USoundBase* ClickSound;	

	UFUNCTION()
	void OnClickedSignInButton();
	UFUNCTION()
	void OnClickedContinueButton();
	UFUNCTION()
	void OnClickedQuitButton();


	UPROPERTY()
	bool IsIDValid = false;

protected:

	virtual void NativeConstruct() override;
};
