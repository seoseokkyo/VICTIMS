
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDLayout.generated.h"


class UButton;
class UBorder;
class UMainLayout;

UCLASS()
class VICTIMS_API UHUDLayout : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* Border_Profile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* Border_Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* Btn_Profile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* Btn_Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UMainLayout* MainLayout;
	
protected:
	UFUNCTION()
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnBtnUIClicked();
};
