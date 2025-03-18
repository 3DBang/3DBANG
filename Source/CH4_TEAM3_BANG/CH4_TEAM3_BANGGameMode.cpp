// Copyright Epic Games, Inc. All Rights Reserved.

#include "CH4_TEAM3_BANGGameMode.h"
#include "CH4_TEAM3_BANGCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACH4_TEAM3_BANGGameMode::ACH4_TEAM3_BANGGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
