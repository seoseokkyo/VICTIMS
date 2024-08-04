// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SeqPlayWidget.generated.h"

/**
 * 
 */

UCLASS()
class VICTIMS_API USeqPlayWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* VideoImage;

    UPROPERTY(meta = (BindWidget))
    class UButton* PlayButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* StopButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* CloseButton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
    class UMediaPlayer* MediaPlayer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
    class UMediaTexture* MediaTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
    class UMaterial* MediaMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
    class UMediaSource* MediaSource;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
    class UMediaSoundComponent* MediaSoundComponent;

    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
    //class UMediaPlaylist* MediaPlaylist;

    UFUNCTION(BlueprintCallable)
    void PlayListIndex(int32 index = 0);

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void OnPlayButtonClicked();

    UFUNCTION()
    void OnStopButtonClicked();

    UFUNCTION()
    void OnCloseButtonClicked();
	
    UFUNCTION()
    void OnMediaEndReached();
};
