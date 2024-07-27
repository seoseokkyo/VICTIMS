
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquippingWeaponWidget.generated.h"


UCLASS()
class VICTIMS_API UEquippingWeaponWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Icon")
	UTexture2D* Knife;
	UPROPERTY(EditAnywhere, Category = "Icon")
	UTexture2D* Axe;
	UPROPERTY(EditAnywhere, Category = "Icon")
	UTexture2D* Pistol;
	UPROPERTY(EditAnywhere, Category = "Icon")
	UTexture2D* Shotgun;
	

	UPROPERTY(meta=(BindWidget))
	class UImage* WeaponIcon;
	UPROPERTY(meta=(BindWidget))
	class UImage* BulletIcon;
	
	UPROPERTY(meta=(BindWidget))
	class UBorder* BulletBorder;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CurrentBullet;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* MaxBullet;

	UPROPERTY(meta=(BindWidget))
	class UBorder* NoBulletNotification;

	UPROPERTY(meta=(BindWidget))
	class UBorder* MainBorder;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetWeaponIcon(FName WeaponName, int32 CurrentBullets, int32 MaxBullets);

	UFUNCTION()
	void HideWeaponIcon();

	UFUNCTION(BlueprintCallable)
	void UpdateBulletUI(int32 Bullets, int32 MaxBullets);

	UFUNCTION()
	void ShowNotification();

};
