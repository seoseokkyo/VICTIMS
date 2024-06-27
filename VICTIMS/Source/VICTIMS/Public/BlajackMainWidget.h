// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
//#include "BlackjackPlyaer.h"
//#include "BlackjackTable.h"
#include "BlajackMainWidget.generated.h"


class ABlackjackPlyaer;
class ABlackjackTable;
/**
 * 
 */

 DECLARE_DELEGATE_OneParam(FPlayerScoreDelegate, int32)
 DECLARE_DELEGATE_OneParam(FDealerScroeDelegate, int32)


UCLASS()
class VICTIMS_API UBlajackMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetPlayer(ABlackjackPlyaer* InputPlayer);

	void SetDealer(ABlackjackTable* InputDealer);

	UFUNCTION()
	void SetPlayerScore(int32 value);

	UFUNCTION()
	void SetDealerScore(int32 value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ABlackjackPlyaer* blackjackPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ABlackjackTable* dealer;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* PlayerScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* DealerScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsChoiceTime=false;

	FPlayerScoreDelegate playerScoreDelegate;
	FDealerScroeDelegate dealerScoreDelegate;
};
