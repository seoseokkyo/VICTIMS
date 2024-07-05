
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPWidget.generated.h"

class UProgressBar;

UCLASS()
class VICTIMS_API UHPWidget : public UUserWidget
{
	GENERATED_BODY()

public: 
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	UProgressBar* hpBar;

	UPROPERTY(EditDefaultsOnly, Category = "Plyaer")
	float HP = 1.0f;

	UFUNCTION()
	void UpdateHPBar(float curHP, float maxHP);

	UPROPERTY()
	class AVICTIMSCharacter* player;

private:
	
	virtual void NativeConstruct() override;
};
