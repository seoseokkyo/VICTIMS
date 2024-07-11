// Copyright Epic Games, Inc. All Rights Reserved.

#include "VICTIMSGameMode.h"
#include "VICTIMSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "AVICTIMSPlayerController.h"
#include "VICTIMSCharacter.h"
#include "BaseWeapon.h"

AVICTIMSGameMode::AVICTIMSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AVICTIMSGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    if (APlayerController* PlayerController = Cast<APlayerController>(Exiting))
    {
		if (AVICTIMSPlayerController* victimsPlayerController = Cast<AVICTIMSPlayerController>(PlayerController))
		{
			if (victimsPlayerController != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Player %s has logged out"), *victimsPlayerController->GetActorNameOrLabel());

				auto playerCheck = Cast<AVICTIMSCharacter>(victimsPlayerController->GetPawn());
				if (playerCheck != nullptr)
				{
					playerCheck->defaultWeapon->BeginDestroy();
				}
			}
		}
    }
}