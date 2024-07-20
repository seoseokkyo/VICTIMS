

#include "TestIDWidget.h"
#include "Components/EditableText.h"
#include "Components/Button.h"
#include "AVICTIMSPlayerController.h"
#include "VICTIMSCharacter.h"

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
			FString strID = ID->GetText().ToString();

			PC->CharacterReference->SavePersonalID(strID);
			PC->SavePersonalID(strID);
			PC->CreateSaveData(strID);
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

		SetOwningPlayer(PC);
		if (PC)
		{
			FString strID = ID->GetText().ToString();

			PC->CharacterReference->SavePersonalID(strID);
			PC->SavePersonalID(strID);
			PC->LoadData(strID);
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
