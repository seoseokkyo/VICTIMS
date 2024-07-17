

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
	if (ID != nullptr)
	{
		AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetGameInstance()->GetFirstLocalPlayerController());
		if (PC)
		{
			PC->CreateSaveData(ID->GetText().ToString());
			if (IsIDValid)
			{
				PC->CloseTestIDWidget();
			}
			else
			{
// 				UE_LOG(LogTemp, Warning, TEXT("Sing In Failed"));
				return;
			}
		}
	}
}

void UTestIDWidget::OnClickedContinueButton()
{
	if (ID != nullptr)
	{
		AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetGameInstance()->GetFirstLocalPlayerController());
		if (PC)
		{
			PC->LoadData(ID->GetText().ToString());
			if (IsIDValid)
			{
				PC->CloseTestIDWidget();
			}
			else
			{
// 				UE_LOG(LogTemp, Warning, TEXT("Continue Failed"));
				return;
			}
		}
	}
}
