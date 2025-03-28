#pragma once

#include "CoreMinimal.h"
#include "Card/BangCardManager.h"
#include "Data/PlayerInformation.h"
#include "GameFramework/GameMode.h"
#include "PlayerController/BangPlayerController.h"
#include "BangGameMode.generated.h"

class UBangCardManager;

UENUM(BlueprintType)
enum class EGameState : uint8
{
	GameOver UMETA(DisplayName = "GameOver"),
	GamePlaying UMETA(DisplayName = "GamePlaying"),
};

UENUM(BlueprintType)
enum class EPlayerTurnState : uint8
{
	DrawCard UMETA(DisplayName = "DrawCard"),
	UseCard UMETA(DisplayName = "UseCard"),
	LooseCard UMETA(DisplayName = "LooseCard")
};

UCLASS()
class CH4_TEAM3_BANG_API ABangGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABangGameMode();
	
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	// 테스트용
	UFUNCTION()
	void StartTest();

	/////////////////
	/// UI 로직
	/// /////////////
	UFUNCTION()
	void UpdatePlayerHUD();

	/////////////////
	/// 통신 로직
	////////////////
	
	// 선택후 남은카드 덱에 돌려놓기
	UFUNCTION()
	void RefundCards(const FPlayerCardCollection& RefundCard);
	// 카드한장을 뽑아서 심볼을 알려준다.
	UFUNCTION()
	void CheckCardSymbol(const uint32& UniqueID, const uint16& CardCount);
	// 플레이어 삭제
	UFUNCTION()
	void ForceUpdate_RemovePlayer(const uint32& UniqueID);
	// 게임 시작
	UFUNCTION()
	void ForceUpdate_StartGame_Real();
	// 게임 중인 플레이어의 정보를 가져온다.
	UFUNCTION()
	void GetPlayerCollection(FPlayerCollection& OutPlayerCollection) const;
	// 심볼로 특정 카드 찾기 (Play Role)
	UFUNCTION()
	void GetCardBySymbol(const FPlayerCardSymbol& Card);
	// 카드 뽑아서 PS에 전달 (플레이어에게 나눠줌)
	UFUNCTION()
	void ForceUpdate_DrawCard(const uint32 UniqueID, const uint16 CardCount);
	// 카드 뽑아서 선택 리스트에 전달 (플레이어에게 나눠줌)
	UFUNCTION()
	void DrawCard(const uint16 CardCount);
	// 카드 뽑아서 전체공개
	UFUNCTION()
	void ShowCard(const uint16 CardCount);
	// 버릴 카드 선택 (시드 케첨 카드 버려서 생명력 회복)
	// 플레이어 사망
	UFUNCTION()
	void PlayerDead(const uint32 UniqueID,
		const ECharacterType PlayerCharacter,
		const EJobType JobType,
		FPlayerCardCollection CardList);
	// 카드 버리기
	UFUNCTION()
	void ForceUpdate_LooseCardFromHanded(const int32 FromUniqueID, const ESymbolType SymbolType, const int32 SymbolNumber, const EDeckType DeckType);
	// 턴 종료
	UFUNCTION()
	void EndTurn(const uint32 UniqueID);

	// 카드 펼치기
	UFUNCTION()
	void ShowTableCardsToAll();

	/**
	 * 지정된 개수의 카드를 뽑고, 클라이언트들에게 알림을 전송합니다.
	 *
	 * @param CardCount 뽑을 카드의 개수를 나타냅니다.
	 */
	void DrawCardsAndNotifyClients();

	/**
	 * 게임 로그 메시지를 클라이언트들에게 전송합니다.
	 *
	 * @param GameLogMessage 전송할 게임 로그 메시지를 나타냅니다.
	 */
	void SendGameLog(const FString& GameLogMessage);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawning")
	float Radius = 500.f;
	
	UPROPERTY()
	TArray<TObjectPtr<ABangPlayerController>> BangPlayerControllers;

	UFUNCTION(BlueprintCallable)
	void SpawnPlayers();

	UFUNCTION(BlueprintCallable)
	APlayerStart* ChooseStartLocation() const;

	/**Test for SpawnActor*/
	UFUNCTION(BlueprintCallable)
	void SpawnPlayerBlue();
	
private:
	// 카드 매니저
	UPROPERTY()
	TObjectPtr<UBangCardManager> CardManager;
	// 개임중인 플레이어 목록
	UPROPERTY()
	FPlayerCollection Players;
	// 현재 플레이어 목록
	UPROPERTY()
	FPlayerCollection LobbyPlayers;
	// 현재 플레이어 인덱스
	int16 PlayerIndex;
	// 현재 게임 상태
	UPROPERTY()
	EGameState CurrentGameState;
	// 현재 턴인 플레이어
	UPROPERTY()
	uint32 CurrentTurnPlayerUniqeID;
	// 현재 플레이어의 턴 상태
	UPROPERTY()
	EPlayerTurnState CurrentPlayerTurnState;

	UPROPERTY()
	uint32 PlayerUniqueIndex = 1;

	// 로비 플레이어 등록
	void AddLobbyPlayer(const uint32& UniqueID, const FString& PlayerNickName, const TObjectPtr<ABangPlayerController>& PlayerController);
	// 로비 플레이어 삭제
	UFUNCTION()
	void RemoveLobbyPlayer(const uint32& UniqueID);
	// 플레이어 자리 배치
	UFUNCTION()
	void ArrangeSeats();
	// 게임 턴 이동 (플레이어 변경)
	UFUNCTION()
	void AdvanceGameTurn();
	// 플레이어 게임 실행 세부 턴
	UFUNCTION()
	void ForceUpdate_AdvancePlayerTurn();
	// 플레이어 자리 섞기
	UFUNCTION()
	void ShuffleSeats(FPlayerCollection& ToShufflePlayers);
    // UniqueID로 PlayerState 받아오기
	UFUNCTION()
	void GetPlayerStatesByUniqueID(const int32& UniqueID, FBangSinglePlayerState& PlayerState_);
	// UniqueID로 PlayerController 받아오기
	UFUNCTION()
	void GetPlayerControllerByUniqueID(const int32& UniqueID, FBangSinglePlayerController& PlayerController_);
	// 현재 턴 플레이어 함정카드 확인
	UFUNCTION()
	void CheckTrapCard();
	
	UFUNCTION(BlueprintCallable)
	void SetUserHP(int32 index);

public:
	UFUNCTION()
	void OpenCamera(uint32 BangPlayerStateID);

	UFUNCTION()
	void CloseCamera();

	TMap<TObjectPtr<ABangPlayerController>, TPair<FVector, FRotator>> PlayersTransfrom;

	UFUNCTION()
	void ReSpawnPlayerAtTurn();

	UFUNCTION(BlueprintCallable)
	void ReSpawnPlayerAtRestart();
	
	UFUNCTION(BlueprintCallable)
	void ReSpawnPlayerAtRestartBluePrint();

	UFUNCTION()
	void NewPossessCharacter(AController* PlayerController, const FVector& SpawnLocation, const FRotator& SpawnRotation);

	UFUNCTION()
	void AtPlayerDie(AController* DeadPlayerController, const FVector& SpawnLocation, const FRotator& SpawnRotation);

	UFUNCTION(BlueprintCallable)
	void DontStopTestBong();

	UFUNCTION(BlueprintCallable)
	void MoveTestBong(int index);
	
	UFUNCTION(BlueprintCallable)
	void UpdateUserHP(int32 index, int32 _HP);

private:
	uint32 ControllerIDAtCameraMode = INDEX_NONE; // Maximum

};
