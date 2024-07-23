// Fill out your copyright notice in the Description page of Project Settings.


#include "CompassWedget.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/UserWidget.h"

void UCompassWedget::NativeConstruct()
{
	Super::NativeConstruct();

	AVICTIMSCharacter* ownerTemp = Cast<AVICTIMSCharacter>(GetOwningPlayer()->GetPawn());
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
	FRotator GetWorldRotation = Rotation->GetComponentRotation();
	// 롤, 피치, 요 값 가져오기
	float PointsX = GetWorldRotation.Roll;
	float PointsY = GetWorldRotation.Pitch;
	float PointsZ = GetWorldRotation.Yaw;

	//위치 계산
	float PositionX = (PointsZ * -1.0f) * 5.0f;
	float PositionY = 0.0f;

	//Points의 캔버스 슬롯 가져옴
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Points->Slot);


	if (CanvasSlot)
	{
		// 위젯의 위치 설정
		CanvasSlot->SetPosition(FVector2D(PositionX, PositionY));
	}

}

void UCompassWedget::SetTimer()
{
	FTimerHandle Handle;
	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &UCompassWedget::SetDirection);
	GetWorld()->GetTimerManager().SetTimer(Handle, Delegate, 0.01f, true);

	
}





