
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
	auto charCheck = Cast<AVICTIMSPlayerController>(GetOwningPlayer());

	if (charCheck)
	{
		auto gi = Cast<UVictimsGameInstance>(GetGameInstance());
		gi->ServerRPC_UpdateMainAddressValue();

		FString strURL = gi->mainAddress + TEXT(":8101");

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("URL : %s"), *strURL));
		UE_LOG(LogTemp, Warning, TEXT("URL : %s"), *strURL);

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("URL : %s"), *strURL));
		UE_LOG(LogTemp, Warning, TEXT("URL : %s"), *strURL);

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("URL : %s"), *strURL));
		UE_LOG(LogTemp, Warning, TEXT("URL : %s"), *strURL);

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("URL : %s"), *strURL));
		UE_LOG(LogTemp, Warning, TEXT("URL : %s"), *strURL);

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("URL : %s"), *strURL));
		UE_LOG(LogTemp, Warning, TEXT("URL : %s"), *strURL);

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("URL : %s"), *strURL));
		UE_LOG(LogTemp, Warning, TEXT("URL : %s"), *strURL);

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("URL : %s"), *strURL));
		UE_LOG(LogTemp, Warning, TEXT("URL : %s"), *strURL);

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("URL : %s"), *strURL));
		UE_LOG(LogTemp, Warning, TEXT("URL : %s"), *strURL);

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("URL : %s"), *strURL));
		UE_LOG(LogTemp, Warning, TEXT("URL : %s"), *strURL);

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("URL : %s"), *strURL));
		UE_LOG(LogTemp, Warning, TEXT("URL : %s"), *strURL);

		charCheck->RequestClientTravel(strURL, FString());
	}
}

void UPlayerDiedWidget::OnClickQuitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, true);
}
