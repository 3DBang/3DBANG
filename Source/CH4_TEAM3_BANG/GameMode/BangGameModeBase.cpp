// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BangGameModeBase.h"
#include "../PlayerController/BangPlayerController.h"
#include "../BangCharacter/BangCharacter.h"


ABangGameModeBase::ABangGameModeBase()
{
	DefaultPawnClass = ABangCharacter::StaticClass();
	PlayerControllerClass = ABangPlayerController::StaticClass();
}