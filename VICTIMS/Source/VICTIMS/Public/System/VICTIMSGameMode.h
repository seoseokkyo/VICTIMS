// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VICTIMSGameMode.generated.h"


UENUM(BlueprintType)
enum class EVictimsNetMode : uint8
{
	NM_Standalone,
	NM_DedicatedServer,
	NM_ListenServer,
	NM_Client,
	NM_MAX,
};


UCLASS(minimalapi)
class AVICTIMSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AVICTIMSGameMode();
	virtual void Logout(AController* Exiting) override;
	virtual void BeginPlay() override;
	virtual void PostLogin(class APlayerController* pc) override;

	void AssignHouseToPlayer(class AVICTIMSPlayerController* NewPlayer);

	UPROPERTY(Replicated)
	TArray<class AShelter*> Houses;

	int32 TotalHouses;

	int32 NextHouseIndex;

	UFUNCTION(BlueprintCallable)
	EVictimsNetMode GetGameNetMode();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	TArray<AVICTIMSPlayerController*> GetPlayers();

	UFUNCTION()
	class AShelter* FindUnOwnedHouse();

	UFUNCTION()
	void ClearHouseOwnership(AVICTIMSPlayerController* PlayerCtrl);
};



