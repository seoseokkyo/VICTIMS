
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerDiedWidget.generated.h"

UCLASS()
class VICTIMS_API UPlayerDiedWidget : public UUserWidget
{
	GENERATED_BODY()
	
public: 
	
	class AVICTIMSPlayerController* pc;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UButton* RespawnButton;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UButton* QuitGameButton;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnClickRespawn();

	UFUNCTION()
	void OnClickQuitGame();
};
