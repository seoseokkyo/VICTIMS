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
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CanvasPanel;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* Compass;

	UPROPERTY(meta = (BindWidget))
	UImage* Points;

	UPROPERTY(meta = (BindWidget))
	UImage* BackGround;

	UPROPERTY(meta = (BindWidget))
	UImage* Arrow;

	UPROPERTY(meta = (BindWidget))
	UImage* Marker;

	UPROPERTY(EditAnywhere, Category="MySettings")
	AVICTIMSCharacter* ownerLocalPlayer;

	UCameraComponent* Rotation;

	UFUNCTION()
    void SetDirection();

	UFUNCTION()
	void SetTimer();

	protected:
	UFUNCTION()
	virtual void NativeConstruct() override;

	public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	private:
	
};
