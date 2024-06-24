
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

struct FInteractableData;
class UInteractionWidget;
class UMainMenu;

UCLASS()
class VICTIMS_API AMainHUD : public AHUD
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UMainMenu> mainMenu_wbp;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UInteractionWidget> interaction_wbp;

	bool bIsMenuVisible;

	//==========================================================================

	
	AMainHUD();

	void DisplayMenu();
	void HideMenu();
	
	void ShowInteractionWidget() const;
	void HideInteractionWidget() const;
	void UpdateInteractionWidget(const FInteractableData* interactableData) const;

protected:

	UPROPERTY()
	UMainMenu* mainMenu;

	UPROPERTY()
	UInteractionWidget* interactionWidget;

	virtual void BeginPlay() override;

	

	
};
