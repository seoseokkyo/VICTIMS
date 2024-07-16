
#include "PlayerDiedWidget.h"
#include "Components/Button.h"
#include "AVICTIMSPlayerController.h"

void UPlayerDiedWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RespawnButton->OnClicked.AddUniqueDynamic(this, &UPlayerDiedWidget::OnClickRespawn);
	QuitGameButton->OnClicked.AddUniqueDynamic(this, &UPlayerDiedWidget::OnClickQuitGame);

}

void UPlayerDiedWidget::OnClickRespawn()
{
}

void UPlayerDiedWidget::OnClickQuitGame()
{
}
