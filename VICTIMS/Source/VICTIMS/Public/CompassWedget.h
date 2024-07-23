// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Player/VICTIMSCharacter.h"
#include "Camera/CameraComponent.h"
#include "CompassWedget.generated.h"

/**
 * 
 */
UCLASS()
class VICTIMS_API UCompassWedget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "MySettings")
	UCanvasPanel* CanvasPanel;

	UPROPERTY(EditAnywhere, Category = "MySettings")
	UCanvasPanel* Compass;

	UPROPERTY(EditAnywhere, Category="MySettings")
	UImage* Points;

	UPROPERTY(EditAnywhere, Category="MySettings")
	UImage* BackGround;

	UPROPERTY(EditAnywhere, Category="MySettings")
	UImage* Arrow;

	UPROPERTY(EditAnywhere, Category="MySettings")
	UImage* Marker;

	UPROPERTY(EditAnywhere, Category="MySettings")
	AVICTIMSCharacter* ownerLocalPlayer;

	UCameraComponent* Rotation;

	UFUNCTION()
    void SetDirection();

	UFUNCTION()
	void SetTimer();

	UFUNCTION()
	void CheckNavPoint();

	UFUNCTION()
	void CheckIfBehind();

	protected:
	UFUNCTION()
	virtual void NativeConstruct() override;

	public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	private:
};
