
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

struct FInteractableData;
class UDataTable;
class UInteractionWidget;
class UMainMenu;
enum class ELayout;

UCLASS()
class VICTIMS_API AMainHUD : public AHUD
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UMainMenu> MainMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;
	
	//======================================================================================
	// FUNCTIONS
	//======================================================================================
	AMainHUD();

	void ToggleMenu(const ELayout layout);
		
	void ShowInteractionWidget() const;
	void HideInteractionWidget() const;
	void UpdateInteractionWidget(const FInteractableData* InteractableData) const;
	
	
	UPROPERTY()
	UMainMenu* MainMenuWidget;
	UPROPERTY()
	UInteractionWidget* InteractionWidget;


protected:

	//======================================================================================
	// FUNCTIONS
	//======================================================================================
	virtual void BeginPlay() override;
};
