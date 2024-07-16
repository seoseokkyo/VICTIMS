

#include "TestIDWidget.h"
#include "Components/EditableText.h"
#include "Components/Button.h"
#include "AVICTIMSPlayerController.h"

void UTestIDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SignInButton->OnClicked.AddUniqueDynamic(this, &UTestIDWidget::OnClickedSignInButton);
	ContinueButton->OnClicked.AddUniqueDynamic(this, &UTestIDWidget::OnClickedContinueButton);

}

void UTestIDWidget::OnClickedSignInButton()
{
	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetGameInstance()->GetFirstLocalPlayerController());
	if (PC)
	{
		PC->CreateSaveData(ID->GetText().ToString());
	}
}

void UTestIDWidget::OnClickedContinueButton()
{
	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetGameInstance()->GetFirstLocalPlayerController());
	if (PC)
	{
		PC->LoadData();
	}
}
