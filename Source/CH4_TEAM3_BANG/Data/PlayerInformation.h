#pragma once

#include "CoreMinimal.h"
#include "Card/BangCardManager.h"
#include "PlayerInformation.generated.h"

USTRUCT(BlueprintType)
struct FPlayerCardSymbol
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ESymbolType SymbolType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 SymbolNumber;

	bool operator==(const FPlayerCardSymbol& Other) const
	{
		return SymbolType == Other.SymbolType && SymbolNumber == Other.SymbolNumber;
	}
};

USTRUCT(BlueprintType)
struct FPlayerCardCollection
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FPlayerCardSymbol> PlayerCards;

	bool operator==(const FPlayerCardCollection& Other) const
	{
		return PlayerCards == Other.PlayerCards;
	}
};

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
	FPlayerCardCollection MyCards;
	
	//장착된 카드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	FPlayerCardCollection EquippedCards;

	bool operator==(const FPlayerInformation& Other) const
	{
		return PlayerUniqueID    == Other.PlayerUniqueID &&
			   PlayerName        == Other.PlayerName &&
			   MaxHealth         == Other.MaxHealth &&
			   CurrentHealth     == Other.CurrentHealth &&
			   RangeToMe         == Other.RangeToMe &&
			   RangeFromMe       == Other.RangeFromMe &&
			   JobCardType       == Other.JobCardType &&
			   CharacterCardType == Other.CharacterCardType &&
			   MyCards           == Other.MyCards &&
			   EquippedCards     == Other.EquippedCards;
	}
};

USTRUCT(BlueprintType)
struct FPlayerCollection
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FPlayerInformation> Players;

	bool operator==(const FPlayerCollection& Other) const
	{
		return Players == Other.Players;
	}

	//플레이어 인덱스를 넣으면 플레이어의 위치를 반환하는 함수
	int32 CalculateDistance(int16 PlayerIndexA, int16 PlayerIndexB)
	{
		int TotalPlayers = this->Players.Num();
		if (TotalPlayers == 0)
		{
			return -1;
		}

		// 인덱스 간 차이 계산
		int16 directDistance = abs(PlayerIndexA - PlayerIndexB);
    
		// 원형 배열에서는 양쪽으로 감싸는 거리가 있을 수 있음
		int16 circularDistance = TotalPlayers - directDistance;
    
		// 두 값 중 작은 값이 실제 거리
		return std::min(directDistance, circularDistance);
	}

	//플레이어 아이디를 넣으면 플레이어 정보를 반환하는 함수
	FPlayerInformation* GetPlayerInformation(uint32 InPlayerUniqueID, uint32& PlayerIndex)
	{
		for (int32 i = 0; i < Players.Num(); ++i)
		{
			if (Players[i].PlayerUniqueID == InPlayerUniqueID)
			{
				PlayerIndex = i;
				return &Players[i];
			}
		}
    
		PlayerIndex = INDEX_NONE; // 찾지 못하면 인덱스를 -1로 설정합니다.
		return nullptr;
	}


};