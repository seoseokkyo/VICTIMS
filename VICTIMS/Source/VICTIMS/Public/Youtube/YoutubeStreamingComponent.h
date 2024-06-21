// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Runtime/Core/Public/Async/AsyncWork.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "YoutubeStreamingComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDelegate_OnUrlLoadedCallback, FString, _url, FString, _title);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDelegate_OnUrlLoaded2Callback, FString, _video_url, FString, _audio_url, FString, _title);

UENUM()
enum VideoQuality
{
	q144_p     UMETA(DisplayName = "144p"),
	q240_p     UMETA(DisplayName = "240p"),
	q360_p     UMETA(DisplayName = "360p"),
	q480_p     UMETA(DisplayName = "480p"),
	q720_p     UMETA(DisplayName = "720p"),
	q1080_p     UMETA(DisplayName = "1080p"),
	quality_p UMETA(DisplayName = "NONE"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VICTIMS_API UYoutubeStreamingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UYoutubeStreamingComponent();
	FHttpModule* Http;
	FString youtubeUrl;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Title;
	
	UFUNCTION(BlueprintCallable, Category = "Youtube Component")
    void GetYoutubeUrls(FString youtube_url);
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "Youtube Component")
	void GetYoutubeUrlsWithQuality(FString youtube_url, TEnumAsByte<VideoQuality> video_quality);
	void OnResponseReceived2(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	UPROPERTY(BlueprintAssignable)
	FDelegate_OnUrlLoadedCallback OnUrlLoaded;

	UPROPERTY(BlueprintAssignable)
	FDelegate_OnUrlLoaded2Callback OnUrlLoaded2;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
