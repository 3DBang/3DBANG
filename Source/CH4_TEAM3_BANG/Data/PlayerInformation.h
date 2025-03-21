#pragma once

#include "CoreMinimal.h"
#include "Card/BangCardManager.h"
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
	FSingleCard JobCard;

	UPROPERTY()
	FSingleCard CharacterCard;


	
};

USTRUCT(BlueprintType)
struct FPlayerCollection
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FPlayerInformation> Players;
};
