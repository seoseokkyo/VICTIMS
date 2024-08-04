

#include "TestIDWidget.h"
#include "Components/EditableText.h"
#include "Components/Button.h"
#include "AVICTIMSPlayerController.h"
#include "VICTIMSCharacter.h"
#include "Shelter.h"
#include "Sound/Soundbase.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "HPWidget.h"
#include "Components/AudioComponent.h"
#include "MyHUD.h"
#include "HUDLayout.h"
#include "MainLayout.h"
#include "CompassWedget.h"
#include "MiniMapWidget.h"

void UTestIDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SignInButton->OnClicked.AddUniqueDynamic(this, &UTestIDWidget::OnClickedSignInButton);
	ContinueButton->OnClicked.AddUniqueDynamic(this, &UTestIDWidget::OnClickedContinueButton);
	QuitButton->OnClicked.AddUniqueDynamic(this, &UTestIDWidget::OnClickedQuitButton);
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

			if (PC->CharacterReference->AssignedHouse)
			{
				PC->CharacterReference->AssignedHouse->SetPlayerName(strID);
			}

			if (IsIDValid)
			{
				PC->CloseTestIDWidget();

				PC->CharacterReference->hpWidget->AddToViewport();

				PC->HUD_Reference->HUDReference->MainLayout->CompassWidget->SetVisibility(ESlateVisibility::Visible);
				PC->HUD_Reference->HUDReference->MainLayout->MiniMapWidget->SetVisibility(ESlateVisibility::Visible);

				if (PC->BGMComp)
				{
					PC->BGMComp->Stop();
				}

				PC->ChatManagerEnable(strID);

			}
			else
			{
				// 				UE_LOG(LogTemp, Warning, TEXT("Sing In Failed"));
				return;
			}
		}
	}
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}
}

void UTestIDWidget::OnClickedContinueButton()
{
	if (ID != nullptr)
	{
		AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetGameInstance()->GetFirstLocalPlayerController());

		if (PC)
		{
			SetOwningPlayer(PC);

			if (PC->IsLocalController())
			{
				FString strID = ID->GetText().ToString();

				if (PC->CharacterReference)
				{
					PC->CharacterReference->SavePersonalID(strID);
				}

				PC->SavePersonalID(strID);
				PC->LoadData(strID);

				if (PC->CharacterReference)
				{
					if (PC->CharacterReference->AssignedHouse)
					{
						PC->CharacterReference->AssignedHouse->SetPlayerName(strID);
					}
				}

				PC->ChatManagerEnable(strID);

// 				if (IsIDValid)
// 				{
// 
// 					PC->CloseTestIDWidget();
// 
// 					PC->CharacterReference->hpWidget->AddToViewport();
// 
// 					PC->HUD_Reference->HUDReference->MainLayout->CompassWidget->SetVisibility(ESlateVisibility::Visible);
// 					PC->HUD_Reference->HUDReference->MainLayout->MiniMapWidget->SetVisibility(ESlateVisibility::Visible);
// 
// 					if (PC->BGMComp)
// 					{
// 						PC->BGMComp->Stop();
// 					}
// 				}
// 				else
// 				{
// 					// 				UE_LOG(LogTemp, Warning, TEXT("Continue Failed"));
// 					return;
// 				}
			}
		}
	}
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}
}

void UTestIDWidget::OnClickedQuitButton()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}

	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, true);
}
