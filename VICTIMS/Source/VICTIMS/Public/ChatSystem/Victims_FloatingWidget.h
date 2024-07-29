// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Victims_FloatingWidget.generated.h"

/**
 * 
 */
class UTextBlock;
class USizeBox;
class UEditableText;
class UWidgetAnimation;
class UButton;
class AController;
class UGmAC_ChatManager;

DECLARE_DELEGATE_OneParam(FBtnContentDel, const AController* InOwnerChatManager);

UCLASS()
class VICTIMS_API UVictims_FloatingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableText> ETB_WhisperInput;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TB_WidgetTitle;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TB_Description;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SB_Accept;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TB_Accept;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> TB_Cancel;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<USizeBox> SB_Cancel;
	
	// TWeakObjectPtr<UGmAC_ChatManager> ChatManagerWeakRef;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn), Category = "Init Settings")
	bool bHasCancelBtn{false};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn), Category = "Init Settings")
	bool bNeedEditableTextBox{false};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn), Category = "Init Settings")
	FText WidgetTitle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn), Category = "Init Settings")
	FText WidgetDescription;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn), Category = "Init Settings")
	FText AcceptBtnText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn), Category = "Init Settings")
	FText CancelBtnText;
	
	FORCEINLINE FBtnContentDel& GetOnClickedAcceptOrCancelBtnDel(const bool InIsAccept)
	{
		return InIsAccept ? OnClickedAcceptBtn : OnClickedCancelBtn;
	}
	
protected:

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> WidgetAppearAnim;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> CheckBtn;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> CancelBtn;

	// UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void RemoveFromParent() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	//~End of UUserWidget Interface

private:

	UFUNCTION()
	void AfterDisappearEvent();

	virtual void AcceptBtnContentLogic();
	UFUNCTION()
	virtual void OnClickedAcceptBtnFunc();

	virtual void CancelBtnContentLogic();
	UFUNCTION()
	virtual void OnClickedCancelBtnFunc();

	UFUNCTION(BlueprintCallable)
	void BeforeRemoveFromParent();

	FBtnContentDel OnClickedAcceptBtn;
	FBtnContentDel OnClickedCancelBtn;
};

