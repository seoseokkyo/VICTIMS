// Fill out your copyright notice in the Description page of Project Settings.


#include "YoutubeStreamingComponent.h"

// Sets default values for this component's properties
UYoutubeStreamingComponent::UYoutubeStreamingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	Http = &FHttpModule::Get();
	// ...
}

void UYoutubeStreamingComponent::GetYoutubeUrls(FString youtube_url)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UYoutubeStreamingComponent::OnResponseReceived);
	FString jsonData = TEXT("{\"url\":\"" + youtube_url + "\"}");
	Request->SetContentAsString(jsonData);
	Request->SetVerb("POST");
	Request->SetURL("http://predictiontr.com/get_youtube_data");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();
}

void UYoutubeStreamingComponent::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	TSharedPtr<FJsonObject> pResponse;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (FJsonSerializer::Deserialize(Reader, pResponse))
	{
		auto video_title = pResponse->GetStringField("title");
		Title=video_title;
		auto video_url = pResponse->GetStringField("url");

		OnUrlLoaded.Broadcast(*video_url, *video_title);
	}
}

void UYoutubeStreamingComponent::GetYoutubeUrlsWithQuality(FString youtube_url, TEnumAsByte<VideoQuality2> video_quality)
{
	FString vq;
	switch (video_quality)
	{
	case q144_p:
		vq = "144p";
		break;
	case q240_p:
		vq = "240p";
		break;
	case q360_p:
		vq = "360p";
		break;
	case q480_p:
		vq = "480p";
		break;
	case q720_p:
		vq = "720p";
		break;
	case q1080_p:
		vq = "1080p";
		break;
	default:
		vq = "360p";
		break;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UYoutubeStreamingComponent::OnResponseReceived2);
	FString jsonData = TEXT("{\"url\":\"" + youtube_url + "\",\"video_format\":\"" + vq + "\"}");
	Request->SetContentAsString(jsonData);
	Request->SetVerb("POST");
	Request->SetURL("http://predictiontr.com/get_youtube_data_v2");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();
}

void UYoutubeStreamingComponent::OnResponseReceived2(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	TSharedPtr<FJsonObject> pResponse;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (FJsonSerializer::Deserialize(Reader, pResponse))
	{
		auto video_title = pResponse->GetStringField("title");
		auto video_url = pResponse->GetStringField("video_url");
		auto audio_url = pResponse->GetStringField("audio_url");

		OnUrlLoaded2.Broadcast(*video_url, *audio_url, *video_title);
	}
}

// Called when the game starts
void UYoutubeStreamingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UYoutubeStreamingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);



	// ...
}

