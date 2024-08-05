
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TabMenuWidget.generated.h"


UCLASS()
class VICTIMS_API UTabMenuWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UButton* QuitGameButton;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UButton* SaveButton;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UButton* CloseMenuButton;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UButton* SettingButton;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UButton* ConfirmButton;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UButton* CancleButton;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class USlider* EditMouseSensivility; 

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* MouseSensivility;


	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UBorder* NotificationBorder;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UBorder* MainBorder;

	UPROPERTY(EditAnywhere, Category = "Interact Sound")
	class USoundBase* ClickSound;

    UPROPERTY(EditAnywhere, Category = "Interact Sound")
	class USoundBase* NotificationSound;

	UFUNCTION()
	void OnMouseSensivilityChanged(float Value);

	UFUNCTION()
	void OnClickQuitGameButton();
	UFUNCTION()
	void OnClickSaveButton();
	UFUNCTION()
	void OnClickCloseMenuButton();
	UFUNCTION()
	void OnClickSettingButton();
	UFUNCTION()
	void OnClickConfirmButton();
	UFUNCTION()
	void OnClickCancleButton();


protected: 

	virtual void NativeConstruct() override;
};
