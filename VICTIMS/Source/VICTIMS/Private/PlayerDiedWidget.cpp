
#include "PlayerDiedWidget.h"
#include "Components/Button.h"
#include "AVICTIMSPlayerController.h"
#include "VictimsGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"

void UPlayerDiedWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RespawnButton->OnClicked.AddUniqueDynamic(this, &UPlayerDiedWidget::OnClickRespawn);
	QuitGameButton->OnClicked.AddUniqueDynamic(this, &UPlayerDiedWidget::OnClickQuitGame);

}

void UPlayerDiedWidget::OnClickRespawn()
{
	auto playerCheck = Cast<AVICTIMSPlayerController>(GetOwningPlayer());
	
	if (playerCheck)
	{
		playerCheck->HideWidgets();

		SetVisibility(ESlateVisibility::Hidden);

		auto gi = Cast<UVictimsGameInstance>(GetGameInstance());

		gi->ServerRPC_RequestRespawn(playerCheck);
	}
}

void UPlayerDiedWidget::OnClickQuitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, true);
}
