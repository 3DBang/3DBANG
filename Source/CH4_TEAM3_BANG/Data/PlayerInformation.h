#pragma once

#include "CoreMinimal.h"
#include "Card/BangCardManager.h"
#include "PlayerInformation.generated.h"

class ABangPlayerState;
class ABangPlayerController;

/////////////////////////////////////////////
/// 플레이어 컨트롤러 & 플레이어 스테이트 구조체
/////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FBangSinglePlayerState
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ABangPlayerState> State;
};

USTRUCT(BlueprintType)
struct FBangSinglePlayerController
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ABangPlayerController> Controller;
};

//////////////////////////////
/// 플레이어 카드 심볼 (통신용)
//////////////////////////////
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
	
	//카드의 심볼과 번호정보로 카드 삭제
	void RemoveCard(const ESymbolType SymbolType, const int32 SymbolNumber)
	{
		for (int16 i = 0; i < PlayerCards.Num(); i++)
		{
			if (PlayerCards[i].SymbolType == SymbolType && PlayerCards[i].SymbolNumber == SymbolNumber)
			{
				PlayerCards.RemoveAt(i);
			}
		}
	}
	
	//카드의 심볼과 번호정보를 플레이어 카드 리스트 안에 넣는 함수
	void AddCardCollectionToPlayerCards(FCardCollection& GivenCards)
	{
		for (const auto& Card : GivenCards.CardList)
		{
			FPlayerCardSymbol SingleSymbol;
			SingleSymbol.SymbolNumber = Card.Card->SymbolNumber;
			SingleSymbol.SymbolType = Card.Card->SymbolType;
			PlayerCards.Add(SingleSymbol);
		}
	}
};

//////////////////////////////
/// 플레이어 정보 (통신용)
//////////////////////////////
USTRUCT(BlueprintType)
struct FPlayerInformation
{
	GENERATED_BODY()

	//플레이어 아이디
	UPROPERTY()
	uint32 PlayerUniqueID = 0;

	//플레이어 이름
	UPROPERTY()
	FString PlayerName = "Default";

	// 플레이어가 가지는 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	int32 MaxHealth = 0;

	// 플레이어 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	int32 CurrentHealth = 0;

	// 총 사거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	int32 GunRange = 0;

	// 사거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	int32 Range = 1;

	// 상대가 날 볼때 추가되는 사거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	int32 CharacterRange = 0; // 말, 캐릭터 특성

	// 내 턴인지 확인
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	bool bIsMyTurn = false;

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

	//함정 카드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	FPlayerCardCollection TrapCards;

	void GetAllCardList(FPlayerCardCollection& OutCardList_)
	{
		for (auto Card : MyCards.PlayerCards)
		{
			OutCardList_.PlayerCards.Add(Card);
		}
		
		for (auto Card : EquippedCards.PlayerCards)
		{
			OutCardList_.PlayerCards.Add(Card);
		}

		for (auto Card : TrapCards.PlayerCards)
		{
			OutCardList_.PlayerCards.Add(Card);
		}
	}

	void GetTrapCardList(FPlayerCardCollection& OutCardList_)
	{
		for (auto Card : TrapCards.PlayerCards)
		{
			OutCardList_.PlayerCards.Add(Card);
		}
	}

	bool operator==(const FPlayerInformation& Other) const
	{
		return PlayerUniqueID    == Other.PlayerUniqueID &&
			   PlayerName        == Other.PlayerName &&
			   MaxHealth         == Other.MaxHealth &&
			   CurrentHealth     == Other.CurrentHealth &&
			   Range			 == Other.Range &&
			   	CharacterRange   == Other.CharacterRange &&
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

	// 선수들 정보 배열
	UPROPERTY()
	TArray<FPlayerInformation> Players;

	// 선수들이 카드를 선택해야할 상황이 오면 선택받아야할 카드 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	FPlayerCardCollection SelectableCards;

	bool operator==(const FPlayerCollection& Other) const
	{
		return Players == Other.Players;
	}

	// 본인 다음 사람 UniqueID
	int32 FindNextPlayer(const int32 PlayerUniqueID)
	{
		for (int16 i = 0; i < Players.Num(); i++)
		{
			if (Players[i].PlayerUniqueID == PlayerUniqueID)
			{
				return Players[(i + 1) % Players.Num()].PlayerUniqueID;
			}
		}
		return 0;
	}

	// 총기 허용 거리 확인
	bool IsBangDistanceAble(const int32 FromUniqueID, const int32 ToUniqueID)
	{
		int32 FromIndex = INDEX_NONE;
		int32 ToIndex = INDEX_NONE;

		for (int32 i = 0; i < Players.Num(); ++i)
		{
			if (Players[i].PlayerUniqueID == FromUniqueID)
				FromIndex = i;
			else if (Players[i].PlayerUniqueID == ToUniqueID)
				ToIndex = i;
		}

		if (FromIndex == INDEX_NONE || ToIndex == INDEX_NONE)
		{
			UE_LOG(LogTemp, Warning, TEXT("[FPlayerCollection::IsDistanceAble] 플레이어 인덱스를 찾을 수 없습니다."));
			return false;
		}

		const int32 DirectDistance = FMath::Abs(ToIndex - FromIndex);
		const int32 ReverseDistance = Players.Num() - DirectDistance;
		const int32 FinalDistance = FMath::Min(DirectDistance, ReverseDistance);

		const int32 AttackerRange = Players[FromIndex].Range;
		const int32 DefenderCamouflage = Players[ToIndex].CharacterRange;
		const int32 GunRange = Players[FromIndex].GunRange;

		const int32 EffectiveRange = AttackerRange + GunRange - DefenderCamouflage;

		return FinalDistance <= EffectiveRange;
	}

	// 허용 거리 확인
	bool IsDistanceAble(const int32 FromUniqueID, const int32 ToUniqueID)
	{
		int32 FromIndex = INDEX_NONE;
		int32 ToIndex = INDEX_NONE;

		for (int32 i = 0; i < Players.Num(); ++i)
		{
			if (Players[i].PlayerUniqueID == FromUniqueID)
				FromIndex = i;
			else if (Players[i].PlayerUniqueID == ToUniqueID)
				ToIndex = i;
		}

		if (FromIndex == INDEX_NONE || ToIndex == INDEX_NONE)
		{
			UE_LOG(LogTemp, Warning, TEXT("[FPlayerCollection::IsDistanceAble] 플레이어 인덱스를 찾을 수 없습니다."));
			return false;
		}

		const int32 DirectDistance = FMath::Abs(ToIndex - FromIndex);
		const int32 ReverseDistance = Players.Num() - DirectDistance;
		const int32 FinalDistance = FMath::Min(DirectDistance, ReverseDistance);

		const int32 AttackerRange = Players[FromIndex].Range;
		const int32 DefenderCamouflage = Players[ToIndex].CharacterRange;

		const int32 EffectiveRange = AttackerRange - DefenderCamouflage;

		return FinalDistance <= EffectiveRange;
	}

	//플레이어 아이디를 넣으면 플레이어 정보를 반환하는 함수
	FPlayerInformation* GetPlayerInformation(const uint32 InPlayerUniqueID)
	{
		for (int32 i = 0; i < Players.Num(); ++i)
		{
			if (Players[i].PlayerUniqueID == InPlayerUniqueID)
			{
				return &Players[i];
			}
		}

		UE_LOG(LogTemp, Error, TEXT("[PlayerInformation::GetPlayerInformation] Player UniqueID not found"));
		return nullptr;
	}

	// 특정 플레이어 삭제
	void RemovePlayer(const uint32 InPlayerUniqueID)
	{
		for (int32 i = 0; i < Players.Num(); ++i)
		{
			if (Players[i].PlayerUniqueID == InPlayerUniqueID)
			{
				Players.RemoveAt(i);
				UE_LOG(LogTemp, Warning, TEXT("[PlayerInformation::GetPlayerInformation] Removed Player with ID: %u"), InPlayerUniqueID);
				return;
			}
		}

		UE_LOG(LogTemp, Error, TEXT("[PlayerInformation::GetPlayerInformation] Player with ID: %u not found. Cannot remove."), InPlayerUniqueID);
	}
};