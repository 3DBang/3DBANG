#pragma once

#include "CoreMinimal.h"
#include "CharacterCard/BangCharacterCard.h"
#include "Data/CardEnums.h"
#include "UObject/Object.h"
#include "BangCardManager.generated.h"

class UBangCardBase;
class UBangCardDataAsset;

/*
	UPROPERTY()
	UBangCardManager* CardManager;
	이렇게 선은해 주셔야지 GC 영향 안받고 살아있습니다.
    게임이 종료되면 사라집니다.

    GameMode -> level 마다
    GameInstance, Singleton -> 계속

    스마트 포인터

    RPC
    NetSerialize
	직렬화는 데이터를 네트워크나 저장소로 전송할 때, 구조체 내부의 값을 일련의 바이트 스트림으로 변환하는 과정입니다.
	즉, 구조체 자체가 그대로 전송되는 게 아니라, 필요한 정보만 최적화된 형태로 패킹되어 전송됩니다.
    GameInstanceSubSystem

	PlayerState, Component 로 뺴서 케릭터 특성 구현 고려
*/

// 카드 객체
USTRUCT(BlueprintType)
struct FSingleCard
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UBangCardBase> Card;

	bool operator==(const FSingleCard& Other) const
	{
		return Card == Other.Card;
	}
};

USTRUCT(BlueprintType)
struct FCardCollection
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSingleCard> CardList;

	bool operator==(const FCardCollection& Other) const
	{
		return CardList == Other.CardList;
	}
};

UCLASS(BlueprintType)
class CH4_TEAM3_BANG_API UBangCardManager : public UObject
{
	GENERATED_BODY()
	
public:
	// 카드 데이터 애셋
	// UPROPERTY 사용시에는
	// 언리얼의 반사 시스템에 등록된 타입만 허용하기 떄문에 스마트 포인터 사용 불가
	// 따라서 원시 포인터 사용
	// 그럼 굳이 블루프린트로 쓰는 이유...?? 데이터 에셋의 동기화?
	// 이후 나온게 TObjectPtr 하지만 내부적으로는 원시포인터로 처리된다.
	// 따라서 올바른 메타데이터를 갖추고 있는지 확인해야 한다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card Manager")
	TObjectPtr<UBangCardDataAsset> CardData;

	// 게임 시작시 호출
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	void PlayBeginByRole();

	// 모든 카드 섞기
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	void ShuffleDeck();

	// 모든 카드 가져오기
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	void GetAllCards();

	// 카드 분배하기
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	void ReorderCards();
	
	// 카드 나눠주기
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	void HandCards(const int CardCount, FCardCollection& OutCards_);

	// 건내준 카드를 다시 사용된 카드 덱에 넣는다
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	void ReorderUsedCards(const FSingleCard HandedCard);
	
	// 건내준 카드를 다시 사용할 카드 덱에 넣는다
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	void ReorderAvailCards(const FSingleCard HandedCard);
	
	// 인원에 맞는 직업카드 추출 로직
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	void GetJobByPlayer(const int PlayerCount, TArray<EJobType>& SelectedCards_);

	// 카드 심볼과 번호로 카드 찾기 IsFromHanded = true -> 건내준 카드목록, false -> 사용된 카드목록
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	void GetCardBySymbolAndNumber(const ESymbolType SymbolType, const int32 SymbolNumber, const bool IsFromHanded, FSingleCard& FoundCard_);

	// DataAsset에서 카드 심볼과 번호로 카드 찾기 (서버에서 호출하면 안됨)
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	void GetCardBySymbolAndNumberFromDataAsset(const ESymbolType SymbolType, const int32 SymbolNumber, FSingleCard& FoundCard_);
	
	// 최상위 단일 케릭터 카드 추출
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	FORCEINLINE_DEBUGGABLE ECharacterType GetCharacterCard() {
		if (CharacterCards.CardList.Num() != 0) {
			const TObjectPtr<UBangCharacterCard> CharacterCard = Cast<UBangCharacterCard>(CharacterCards.CardList[0].Card);
			CharacterCards.CardList.RemoveAt(0);
			return CharacterCard->CharacterType;
		}
		return ECharacterType::None;
	}

private:
	// 모든 카드 저장 배열
	UPROPERTY()
	FCardCollection AllCards;

	// 개별 카드 배열
	// 계속 사용
	UPROPERTY()
	FCardCollection PassiveCards;
	UPROPERTY()
	FCardCollection ActiveCards;
	// 게임 시작시 최초 1회 배포
	UPROPERTY()
	FCardCollection CharacterCards;
	UPROPERTY()
	FCardCollection JobCards;

	// 사용된 카드 덱
	UPROPERTY()
	FCardCollection UsedCards;
	UPROPERTY()
	FCardCollection HandedCards;

	// 사용가능 카드 덱
	UPROPERTY()
	FCardCollection AvailCards;

	// 카드 타입별 저장
	UPROPERTY()
	TMap<ECardType, FCardCollection> CardDeckByType;

	UFUNCTION()
	void ShuffleCards(FCardCollection& Cards);

	template<typename T>
	static void ShuffleArray(TArray<T>& Array)
	{
		const int32 Count = Array.Num();
		if (Count <= 1) return;

		for (int32 i = Count - 1; i > 0; --i)
		{
			const int32 RandomIndex = FMath::RandRange(0, i);
			Array.Swap(i, RandomIndex);
		}
	}
};
