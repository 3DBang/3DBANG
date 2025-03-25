#pragma once

#include "CoreMinimal.h"
#include "Data/PlayerInformation.h"
#include "Card/BangCardManager.h"
#include "GameFramework/PlayerState.h"
#include "BangPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTurnStartDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerInfoUpdated, FPlayerCollection, PlayerCollection);

UCLASS()
class CH4_TEAM3_BANG_API ABangPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ABangPlayerState();

	virtual void BeginPlay() override;

	// Delegate
	UPROPERTY(BlueprintAssignable, Category = "Delegate|Event")
	FOnPlayerInfoUpdated FOnPlayerInfoUpdated;

	// PlayerState에서 사용하는 플레이어 정보 저장용
	UPROPERTY(ReplicatedUsing = OnRep_PlayerInfo)
	FPlayerCollection PlayerInfo;

	// 본인 UniqueID
	UPROPERTY()
	uint32 PlayerUniqueID = 0;

	// 플레이어 Info가 서버에서 변경됐을떄 호출되는 함수
	UFUNCTION()
	void OnRep_PlayerInfo();

	// 플레이어 체력 감소
	UFUNCTION()
	void LoosePlayerHealth(const uint32& TargetUniqueID, int32 Amount);

	// 플레이어 체력 증가
	UFUNCTION()
	void GainPlayerHealth(const uint32& TargetUniqueID, int32 Amount);

	// 컨트롤러가 카드 조회
	UFUNCTION()
	void GetCard(const int32 InPlayerUniqueID, FCardCollection& OutCardCollection);

	// 플레이어 인포를 받아와서 하는 카드조회
	void GetCard(int32 InPlayerUniqueID, FCardCollection& OutCardCollection, FPlayerCollection _PlayerInfo);

	// 컨트롤러가 카드타입 조회
	UFUNCTION()
	void GetCardType(const int32 InPlayerUniqueID, const FSingleCard& Card, EActiveType& OutActiveType, EPassiveType& OutPassiveType);

	// 컨트롤러가 카드 사용할때 호출
	// PC -> PS
	UFUNCTION()
	void UseCard(const int32 FromUniqueID, const FSingleCard& SingleCard, const int32 ToUniqueID);

	// 컨트롤러가 전체 카드 사용할때 호출
	UFUNCTION()
	void UseCardToAll(const int32 FromUniqueID, FSingleCard SingleCard);

	// 컨트롤러가 카드 버릴때 호출
	UFUNCTION()
	void RestoreCard(const int32 FromUniqueID, FSingleCard SingleCard);
	
	// 턴 시작
	UFUNCTION()
	void StartTurn(const int32 InPlayerUniqueID, FCardCollection& DrawCards);
	
	// 턴 종료
	UFUNCTION()
	void EndTurn(const int32 InPlayerUniqueID);

	// 카드사용에 대한 응답 (PS -> PS)
	UFUNCTION()
	void UseCardReturn(const int32& FromUniqueID, const FPlayerCardSymbol& SingleCard, const int32& ToUniqueID, const EActiveType& ActiveType, const EPassiveType& PassiveType);

	// 대상의 PS를 찾는다
	UFUNCTION()
	void FindTargetPlayerState(const uint32 TargetUniqueID, FBangSinglePlayerState& OutPlayerState) const;

	// 패시브 카드 중복 장착 방지용
	UFUNCTION()
	bool CheckIsCardAble(const int32 FromUniqueID, const FSingleCard& SingleCard);
	
	/////////////////////
	/// 서버통신
	/////////////////////
	UFUNCTION(Server, Reliable)
	void Server_EndTurn(const int32 InPlayerUniqueID);
	
	UFUNCTION(Server, Reliable)
	void Server_UseCard(
		const int32 FromUniqueID,
		const ESymbolType SymbolType,
		const int32 SymbolNumber,
		const EDeckType DeckType);

	// 플레이어 사망 처리
	UFUNCTION(Server, Reliable)
	void Server_PlayerDead(const int32 FromUniqueID);
	
	// 플레이어 카드 뽑기
	UFUNCTION(Server, Reliable)
	void Server_DrawCard(const uint32 FromUniqueID, const uint16 CardCount, const bool bIsForce);

	// 카드 심볼 확인
	UFUNCTION(Server, Reliable)
	void Server_CheckCardSymbol(const uint32& FromUniqueID, const uint16& CardCount);
	
	// PlayerInfo 동기화 PlayerState에서 값 변경 후 호출해야함
	UFUNCTION(Server, Reliable)
	void Server_SetPlayerInfo(const FPlayerCollection& NewInfo);

	UFUNCTION(Client, Reliable)
	void Client_SetUniqueId(const uint32& FromPlayerUniqueID);
	
	// 카드 심볼 확인 리턴
	UFUNCTION(Client, Reliable)
	void Client_CheckCardSymbolReturn(const uint32& FromUniqueID, const FPlayerCardCollection& PlayerCardCollection);

	void HandlePlayerInfoUpdated();
private:
	// 카드매니저
	UPROPERTY()
	TObjectPtr<UBangCardManager> CardManager;
	
	// 심볼, 번호 정보로 카드리스트에서 카드를 찾아온다.
	UFUNCTION()
	FCardCollection GetCardListFromCardManager(const FPlayerInformation& Info) const;

	// 디버그용
	UFUNCTION()
	FString FPlayerInformationToString(const FPlayerInformation& Info);
	UFUNCTION()
	FString FPlayerCollectionToString(const FPlayerCollection& Collection);
	
};


