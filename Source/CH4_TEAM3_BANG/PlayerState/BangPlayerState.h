#pragma once

#include "CoreMinimal.h"
#include "Data/PlayerInformation.h"
#include "Card/BangCardManager.h"
#include "GameFramework/PlayerState.h"
#include "BangPlayerState.generated.h"

UCLASS()
class CH4_TEAM3_BANG_API ABangPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ABangPlayerState();

	virtual void BeginPlay() override;

	// PlayerState에서 사용하는 플레이어 정보 저장용
	UPROPERTY(ReplicatedUsing = OnRep_PlayerInfo)
	FPlayerCollection PlayerInfo;

	// 플레이어 Info가 서버에서 변경됐을떄 호출되는 함수
	UFUNCTION()
	void OnRep_PlayerInfo();

	// 컨트롤러가 카드 조회 호출
	UFUNCTION()
	void GetCard(const int32 InPlayerUniqueID, FCardCollection& OutCardCollection);

	// 컨트롤러가 카드 사용할때 호출
	UFUNCTION()
	void UseCard(const int32 FromUniqueID, FSingleCard SingleCard, const int32 ToUniqueID);

	// 컨트롤러가 전체 카드 사용할때 호출
	UFUNCTION()
	void UseCardToAll(const int32 FromUniqueID, FSingleCard SingleCard);

	// 컨트롤러가 카드 버릴때 호출
	UFUNCTION()
	void RestoreCard(const int32 FromUniqueID, FSingleCard SingleCard);

	// 턴 종료
	UFUNCTION()
	void EndTurn(const int32 InPlayerUniqueID);

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
	void Server_DrawCard(const uint32 FromUniqueID, const uint16 CardCount);
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


