// Copyright Epic Games, Inc. All Rights Reserved.

#include "VICTIMSGameMode.h"
#include "VICTIMSCharacter.h"
#include "UObject/ConstructorHelpers.h"

AVICTIMSGameMode::AVICTIMSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
