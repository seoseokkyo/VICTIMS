
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ELayout.h"
#include "MyHUD.generated.h"

class UHUDLayout;
struct FWidgetsLayoutBP;
class UDataTable;
UCLASS()
class VICTIMS_API AMyHUD : public AHUD
{
	GENERATED_BODY()
public:
	UFUNCTION()
	bool IsAnyWidgetVisible();

	UFUNCTION()
	void ToggleWindow(const ELayout Layout);

	FWidgetsLayoutBP* GetWidgetBPClass(FName Name);
	
	UFUNCTION()
	UUserWidget* GenerateInteractWidget(FText Text);

	UPROPERTY(EditAnywhere, Category = "Default")
	UHUDLayout* HUDReference;
	
	UUserWidget* CustomCreateWidget(FName Name);

protected:
	AMyHUD();
	
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Default")
	UDataTable* WidgetDB;
	
	UUserWidget* CreateWidgetFromDataTable(const UDataTable* WidgetTable, FWidgetsLayoutBP*& NewWidgetData, FName Name);
};
