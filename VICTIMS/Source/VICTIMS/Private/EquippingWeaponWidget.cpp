

#include "EquippingWeaponWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "AVICTIMSPlayerController.h"
#include "InventoryManagerComponent.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Engine/Texture2D.h>

void UEquippingWeaponWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MainBorder->SetVisibility(ESlateVisibility::Hidden);
	NoBulletNotification->SetVisibility(ESlateVisibility::Hidden);
}

void UEquippingWeaponWidget::SetWeaponIcon(FName WeaponName, int32 CurrentBullets, int32 MaxBullets)
{
	MainBorder->SetVisibility(ESlateVisibility::Visible);
	FName tempName = WeaponName;
	if (tempName == FName("ID_Knife"))
	{
		if (Knife)
		{
			WeaponIcon->SetBrushFromTexture(Knife);
			BulletBorder->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	if (tempName == FName("ID_Axe"))
	{
		if (Axe)
		{
			WeaponIcon->SetBrushFromTexture(Axe);
			BulletBorder->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	if (tempName == FName("ID_Pistol"))
	{
		if (Pistol)
		{
			WeaponIcon->SetBrushFromTexture(Pistol);
			BulletBorder->SetVisibility(ESlateVisibility::Visible);
			CurrentBullet->SetText(FText::AsNumber(CurrentBullets));
			MaxBullet->SetText(FText::AsNumber(MaxBullets));

		}
	}
	if (tempName == FName("ID_ShotGun"))
	{
		if (Shotgun)
		{
			WeaponIcon->SetBrushFromTexture(Shotgun);
			BulletBorder->SetVisibility(ESlateVisibility::Visible);
			CurrentBullet->SetText(FText::AsNumber(CurrentBullets));
			MaxBullet->SetText(FText::AsNumber(MaxBullets));
		}
	}
}

void UEquippingWeaponWidget::HideWeaponIcon()
{
	MainBorder->SetVisibility(ESlateVisibility::Hidden);
}

void UEquippingWeaponWidget::UpdateMaxBullet(int32 Bullets)
{
	MaxBullet->SetText(FText::AsNumber(Bullets));
}

void UEquippingWeaponWidget::UpdateCurrentBullet(int32 Bullets)
{
	ClientRPC_UpdateCurrentBullet(Bullets);
}

void UEquippingWeaponWidget::ClientRPC_UpdateCurrentBullet_Implementation(int32 Bullets)
{
	CurrentBullet->SetText(FText::AsNumber(Bullets));
}

void UEquippingWeaponWidget::ShowNotification()
{
	NoBulletNotification->SetVisibility(ESlateVisibility::Visible);
	FTimerHandle T; 
	GetWorld()->GetTimerManager().ClearTimer(T);
	GetWorld()->GetTimerManager().SetTimer(T, [&](){
		NoBulletNotification->SetVisibility(ESlateVisibility::Hidden);
	}, 1.0f, false);
}
