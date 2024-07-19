// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VICTIMSGameMode.generated.h"

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

	TArray<class AShelter*> Houses;
	int32 TotalHouses;
	int32 NextHouseIndex;

};



