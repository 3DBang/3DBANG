#pragma once

#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Card/BaseCard/BangCardBase.h"
#include "PlayerInformation.generated.h"

USTRUCT(BlueprintType)
struct FPlayerInformation
{
	GENERATED_BODY()

	UPROPERTY()
	uint32 PlayerUniqueID;

	UPROPERTY()
	FString PlayerName;

	UPROPERTY()
	TObjectPtr<UBangCardBase> JobCard;

	UPROPERTY()
	TObjectPtr<UBangCardBase> CharacterCard;


	
};

USTRUCT(BlueprintType)
struct FPlayerCollection
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FPlayerInformation> Players;
};
