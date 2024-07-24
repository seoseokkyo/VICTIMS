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
	// ��, ��ġ, �� �� ��������
	float PointsX = GetWorldRotation.Roll;
	float PointsY = GetWorldRotation.Pitch;
	float PointsZ = GetWorldRotation.Yaw;

	//��ġ ���
	float PositionX = (PointsZ * -1.0f) * 5.0f;
	float PositionY = 0.0f;

	//Points�� ĵ���� ���� ������
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Points->Slot);


	if (CanvasSlot)
	{
		// ������ ��ġ ����
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





