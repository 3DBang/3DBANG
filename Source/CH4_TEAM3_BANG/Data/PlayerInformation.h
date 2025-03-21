#pragma once

#include "CoreMinimal.h"
#include "Card/CharacterCard/BangCharacterCard.h"
#include "Card/JobCard/BangJobCard.h"
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
	EJobType JobCardType;

	UPROPERTY()
	ECharacterType CharacterCardType;


	
};

USTRUCT(BlueprintType)
struct FPlayerCardCollection
{
	GENERATED_BODY()

	UPROPERTY()
	ESymbolType SymbolType;

	UPROPERTY()
	int32 SymbolNumber;
};

USTRUCT(BlueprintType)
struct FPlayerCollection
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FPlayerInformation> Players;
};
