#pragma once

#include "CoreMinimal.h"
#include "Card/BangCardManager.h"
#include "PlayerInformation.generated.h"

USTRUCT(BlueprintType)
struct FPlayerInformation
{
	GENERATED_BODY()

	//플레이어 아이디
	UPROPERTY()
	uint32 PlayerUniqueID;

	//플레이어 이름
	UPROPERTY()
	FString PlayerName;

	// 플레이어가 가지는 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	int32 MaxHealth;

	// 플레이어 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	int32 CurrentHealth;

	// 나를 볼 때 사거리 (다른 플레이어 기준)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	int32 RangeToMe;

	// 내가 볼 때 사거리 (내 기준)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	int32 RangeFromMe;

	//직업 타입
	UPROPERTY()
	EJobType JobCardType;

	//캐릭터 타입
	UPROPERTY()
	ECharacterType CharacterCardType;
	
	//보유한 카드(사용가능한 카드)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	FCardCollection MyCards;
	
	//장착된 카드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	FCardCollection EquippedCards;
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
