// Fill out your copyright notice in the Description page of Project Settings.


#include "SeqPlayWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "MediaPlayer.h"
#include "MediaSoundComponent.h"
#include "MediaTexture.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "MediaPlaylist.h"
#include "MediaSource.h"
#include "AVICTIMSPlayerController.h"

void USeqPlayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PlayButton)
	{
		PlayButton->OnClicked.AddDynamic(this, &USeqPlayWidget::OnPlayButtonClicked);
	}

	if (StopButton)
	{
		StopButton->OnClicked.AddDynamic(this, &USeqPlayWidget::OnStopButtonClicked);
	}

	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &USeqPlayWidget::OnCloseButtonClicked);
	}

	//if (MediaPlayer && MediaTexture && VideoImage)
	//{
	//	//FSlateBrush brush;
	//	//brush.SetResourceObject(MediaTexture);
	//	//VideoImage->SetBrush(brush);
	//
	//	if (MediaMaterial)
	//	{
	//		UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(MediaMaterial, this);
	//		if (MaterialInstance)
	//		{
	//			MaterialInstance->SetTextureParameterValue(FName("Texture"), MediaTexture);
	//			VideoImage->SetBrushFromMaterial(MaterialInstance);
	//		}
	//	}
	//

	if (MediaSoundComponent == nullptr)
	{
		//MediaSoundComponent = NewObject<UMediaSoundComponent>(this);
		if (auto pc = Cast<AVICTIMSPlayerController>(GetOwningPlayer()))
		{
			MediaSoundComponent = pc->MediaSoundComponent;
		}
	}

	if (MediaSoundComponent)
	{
		MediaSoundComponent->SetMediaPlayer(MediaPlayer);
		MediaSoundComponent->RegisterComponent();
	}

	//
	//	MediaPlayer->OnEndReached.AddDynamic(this, &USeqPlayWidget::OnMediaEndReached);
	//}
}

void USeqPlayWidget::PlayListIndex(int32 index)
{
	if (MediaPlayer == nullptr)
	{
		return;
	}

	MediaPlayer->OpenSource(MediaSource);

	//MediaPlayer->Play();

	//MediaPlayer->PlayAndSeek();

	//if (MediaPlaylist && MediaPlaylist->Num() > index)
	//{
	//    MediaPlayer->OpenPlaylistIndex(MediaPlaylist, index);
	//
	//}
}

void USeqPlayWidget::OnPlayButtonClicked()
{
	if (MediaPlayer && !MediaPlayer->IsPlaying())
	{
		MediaPlayer->Play();
	}
}

void USeqPlayWidget::OnStopButtonClicked()
{
	if (MediaPlayer && MediaPlayer->IsPlaying())
	{
		MediaPlayer->Pause();
	}
}

void USeqPlayWidget::OnCloseButtonClicked()
{
	if (MediaPlayer)
	{
		MediaPlayer->Close();
	}

	this->RemoveFromParent();
}

void USeqPlayWidget::OnMediaEndReached()
{
	OnCloseButtonClicked();
}