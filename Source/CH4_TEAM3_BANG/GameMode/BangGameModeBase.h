#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BangGameModeBase.generated.h"

class ABangPlayerState;
class ABangGameState;
class FCardData;
enum class EPlayerRole : uint8;
 
UCLASS()
class CH4_TEAM3_BANG_API ABangGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
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

	void PrintPlayerStats();

	FTimerHandle TimerHandle;

	// 현재 턴 알림
	//UFUNCTION(NetMulticast, Reliable)
	//void Multicast_NotifyTurn(int32 PlayerIndex);

	// 게임 종료 체크
	void CheckGameEnd();

	//UFUNCTION()
	//void HandleAttack(ABangPlayerState* Attacker, ABangPlayerState* Defender);

	//UFUNCTION()
	//void HandlePlayerDeath(ABangPlayerState* DeadPlayer);

protected:
	// 현재 턴 플레이어의 인덱스
	int32 CurrentTurnPlayerIndex;

	int32 NumSheriffs;
	int32 NumDeputies;
	int32 NumOutlaws;
	int32 NumRenegades;
};
