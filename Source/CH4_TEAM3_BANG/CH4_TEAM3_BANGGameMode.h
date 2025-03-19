#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CH4_TEAM3_BANGGameMode.generated.h"

// 전방 선언
class ACH4_TEAM3_PlayerState;
class ACH4_TEAM3_BANGGameState;
class FCardData;
enum class EPlayerRole : uint8;

UCLASS(minimalapi)
class ACH4_TEAM3_BANGGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACH4_TEAM3_BANGGameMode();

	//게임 시작
	UFUNCTION(Server, Reliable)
	void StartGame();

	// 게임 시작 멀티캐스트
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartGame();

	// 역할 배정
	void AssignRoles();

	// 역할 섞기
	void ShuffleRoles(TArray<EPlayerRole>& Roles);

	// 첫 번째 턴 시작 (보안관부터)
	void StartFirstTurn();

	// 현재 턴 플레이어 턴 시작
	void StartTurn(int32 PlayerIndex);

	// 턴 종료
	void EndTurn();

	// 다음 플레이어로 턴 넘기기
	void NextTurn();

	// 현재 턴 알림
	//UFUNCTION(NetMulticast, Reliable)
	//void Multicast_NotifyTurn(int32 PlayerIndex);

	// 게임 종료 체크
	void CheckGameEnd();

	//UFUNCTION()
	//void HandleAttack(ACH4_TEAM3_PlayerState* Attacker, ACH4_TEAM3_PlayerState* Defender);

	//UFUNCTION()
	//void HandlePlayerDeath(ACH4_TEAM3_PlayerState* DeadPlayer);

protected:
	// 현재 턴 플레이어의 인덱스
	int32 CurrentTurnPlayerIndex;
};