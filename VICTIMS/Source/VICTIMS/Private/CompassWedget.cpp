// Fill out your copyright notice in the Description page of Project Settings.


#include "CompassWedget.h"
#include "Blueprint/UserWidget.h"

void UCompassWedget::NativeConstruct()
{
	Super::NativeConstruct();

	AVICTIMSCharacter* ownerTemp = Cast<AVICTIMSCharacter>(GetOwningLocalPlayer());
	if (ownerTemp!=nullptr)
		{
			ownerLocalPlayer = ownerTemp;

			Rotation = ownerLocalPlayer->GetFollowCamera();

			SetTimer();
			
		}
}

void UCompassWedget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UCompassWedget::SetDirection()
{

	FRotator RotTemp= Rotation->GetComponentRotation();
	//여기 하던 중이었음
	//Points->


}

void UCompassWedget::SetTimer()
{
	FTimerHandle Handle;
	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &UCompassWedget::SetDirection);
	GetWorld()->GetTimerManager().SetTimer(Handle, Delegate, 0.1f, true);

	
}

void UCompassWedget::CheckNavPoint()
{
}

void UCompassWedget::CheckIfBehind()
{
}
