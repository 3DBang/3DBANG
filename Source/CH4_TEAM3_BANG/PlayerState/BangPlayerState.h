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


