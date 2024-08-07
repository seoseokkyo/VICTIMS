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
#include "Components/AudioComponent.h"

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

	if (InitMedia())
	{
		if (auto pc = Cast<AVICTIMSPlayerController>(GetOwningPlayer()))
		{
			pc->bIsShowUI = true;
			pc->EnableUIMode();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InitMedia Failed"));
	}
}

bool USeqPlayWidget::InitMedia()
{
	if (MediaPlayer && MediaTexture && VideoImage)
	{
		FSlateBrush brush;
		brush.SetResourceObject(MediaTexture);
		VideoImage->SetBrush(brush);

		MediaPlayer->OnEndReached.AddDynamic(this, &USeqPlayWidget::OnMediaEndReached);

		return true;
	}
	else
	{
		return false;
	}
}

bool USeqPlayWidget::PlayMedia()
{
	if (MediaPlayer && MediaSource)
	{
		if (MediaPlayer->OpenSource(MediaSource))
		{
			if (auto pc = Cast<AVICTIMSPlayerController>(GetOwningPlayer()))
			{
				pc->SeqSoundComp = UGameplayStatics::CreateSound2D(GetWorld(), SeqSound);
				pc->SeqSoundComp->Play();
			}

			if (!MediaPlayer->IsPlaying())
			{
				MediaPlayer->Play();
			}

			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MediaPlayer->OpenSource Failed"));
			return false;
		}
	}

	return false;
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

	if (auto pc = Cast<AVICTIMSPlayerController>(GetOwningPlayer()))
	{
		pc->bIsShowUI = false;
		pc->DisableUIMode();

		if (pc->SeqSoundComp)
		{
			pc->SeqSoundComp->Stop();
		}

		pc->SetSeqPlayingState(false);
	}

	this->RemoveFromParent();
}

void USeqPlayWidget::OnMediaEndReached()
{
	OnCloseButtonClicked();
}