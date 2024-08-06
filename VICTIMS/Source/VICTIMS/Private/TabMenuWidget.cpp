

#include "TabMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/Slider.h"
#include "AVICTIMSPlayerController.h"
#include "VICTIMSCharacter.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>


void UTabMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	QuitGameButton->OnClicked.AddDynamic(this, &UTabMenuWidget::OnClickQuitGameButton);
	QuitGameButton->IsFocusable = false;
	SaveButton->OnClicked.AddDynamic(this, &UTabMenuWidget::OnClickSaveButton);
	SaveButton->IsFocusable = false;
	CloseMenuButton->OnClicked.AddDynamic(this, &UTabMenuWidget::OnClickCloseMenuButton);
	CloseMenuButton->IsFocusable  = false;
	SettingButton->OnClicked.AddDynamic(this, &UTabMenuWidget::OnClickSettingButton);
	SettingButton->IsFocusable = false;
	ConfirmButton->OnClicked.AddDynamic(this, &UTabMenuWidget::OnClickConfirmButton);
	ConfirmButton->IsFocusable = false;
	CancleButton->OnClicked.AddDynamic(this, &UTabMenuWidget::OnClickCancleButton);
	CancleButton->IsFocusable = false;
	EditMouseSensivility->OnValueChanged.AddDynamic(this, &UTabMenuWidget::OnMouseSensivilityChanged);
	EditMouseSensivility->SetValue(FCString::Atof(*MouseSensivility->GetText().ToString()));

	NotificationBorder->SetVisibility(ESlateVisibility::Hidden);
	
}

void UTabMenuWidget::OnMouseSensivilityChanged(float Value)
{
	MouseSensivility->SetText(FText::AsNumber(Value));
}

void UTabMenuWidget::OnClickQuitGameButton()
{
	NotificationBorder->SetVisibility(ESlateVisibility::Visible);
	if (NotificationSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), NotificationSound);
	}
}

void UTabMenuWidget::OnClickSaveButton()
{
	AVICTIMSPlayerController* pc = Cast<AVICTIMSPlayerController>(GetGameInstance()->GetFirstLocalPlayerController());
	if(pc)
	{
		if (ClickSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
		}
		pc->SaveData();
	}
}

void UTabMenuWidget::OnClickCloseMenuButton()
{
	MainBorder->SetVisibility(ESlateVisibility::Hidden);
	AVICTIMSPlayerController* pc = Cast<AVICTIMSPlayerController>(GetGameInstance()->GetFirstLocalPlayerController());
	if (pc)
	{
		if (ClickSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
		}
		pc->bShowMouseCursor = false;
		pc->SetInputMode(FInputModeGameOnly());
	}
}

void UTabMenuWidget::OnClickSettingButton()
{
	AVICTIMSPlayerController* pc = Cast<AVICTIMSPlayerController>(GetGameInstance()->GetFirstLocalPlayerController());
	if (pc)
	{
		if (ClickSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
		}
		float value = EditMouseSensivility->GetValue();
		pc->CharacterReference->SetMouseSensivility(value);

		pc->SaveData();
	}
}

void UTabMenuWidget::OnClickConfirmButton()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, true);
}

void UTabMenuWidget::OnClickCancleButton()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}
	NotificationBorder->SetVisibility(ESlateVisibility::Hidden);
}
