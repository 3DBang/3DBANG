#pragma once

#include "CoreMinimal.h"
#include "Card/BangCardManager.h"
#include "Data/PlayerInformation.h"
#include "GameFramework/GameMode.h"
#include "PlayerController/BangPlayerController.h"
#include "PlayerState/BangPlayerState.h"
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

USTRUCT(BlueprintType)
struct FBangPlayerStateCollection
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ABangPlayerState> State;
};

USTRUCT(BlueprintType)
struct FBangPlayerControllerCollection
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ABangPlayerController> Controller;
};

UCLASS()
class CH4_TEAM3_BANG_API ABangGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABangGameMode();
	
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 로비 플레이어 등록
	UFUNCTION()
	void AddPlayer(const uint32& UniqueID);
	// 로비 플레이어 삭제
	UFUNCTION()
	void RemovePlayer(const uint32& UniqueID);
	// 게임 시작
	UFUNCTION()
	void StartGame();
	// 단일 카드 사용 (Play Role)
	UFUNCTION() // 실제 객체 주소가 넘어가는지 확인 필요
	void UseCard(
		const uint32 UniqueID, // 사용한 사람의 아이디
		const FPlayerCardSymbol& Card, // 카드 정보
		const EActiveType ActiveType, // 액티브 타입
		const EPassiveType PassiveType, // 패시브 타입
		const ECharacterType CharacterType, // 캐릭터 타입
		const uint32 ToUniqueID, // 대상
		const ECharacterType ToCharacterType // 대상 캐릭터 타입
		) const;
	// 버릴 카드 선택 (Play Role)
	UFUNCTION()
	void LooseCard(const FCardCollection CardList);
	// 버릴 카드 선택 (시드 케첨 카드 버려서 생명력 회복)
	UFUNCTION()
	void LooseSidKetchumCard(const FCardCollection CardList);
	// 플레이어 사망
	UFUNCTION()
	void PlayerDead(const uint32 UniqueID,
		const ECharacterType PlayerCharacter,
		const EJobType JobType,
		const FCardCollection CardList);
	// 카드 버리기
	UFUNCTION()
	void LooseCardFromHanded(const ESymbolType SymbolType, const int32 SymbolNumber, const bool IsToUsed) const;
	// 턴 종료
	UFUNCTION()
	void EndTurn(const uint32 UniqueID, ECharacterType PlayerCharacter);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawning")
	float Radius = 500.f;

	// 원멍 : 플레이어 컨트롤러 임시저장 
	TArray<APlayerController*> PlayerControllers;

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
	FString CurrentPlayerTurn;
	// 현재 플레이어의 턴 상태
	UPROPERTY()
	EPlayerTurnState CurrentPlayerTurnState;

	// 플레이어 자리 배치
	UFUNCTION()
	void ArrangeSeats();
	// 게임 턴 이동 (플레이어 변경)
	UFUNCTION()
	void AdvanceGameTurn();
	// 플레이어 게임 실행 세부 턴
	UFUNCTION()
	void AdvancePlayerTurn();
	// 플레이어 자리 섞기
	UFUNCTION()
	void ShuffleSeats(FPlayerCollection& ToShufflePlayers) const;
    // UniqueID로 PlayerState 받아오기
	UFUNCTION()
	void GetPlayerStatesByUniqueID(const int32& UniqueID, FBangPlayerStateCollection& PlayerState_);
	// UniqueID로 PlayerController 받아오기
	UFUNCTION()
	void GetPlayerControllerByUniqueID(const int32& UniqueID, FBangPlayerControllerCollection& PlayerController_);

	// 강탈카드사용 (Play Role)
	UFUNCTION()
	void UsePanicCard(const EActiveType ActiveType, const EPassiveType PassiveType);
	// 캣벌로우사용 (Play Role)
	UFUNCTION()
	void UseCatBalouCard(const EActiveType ActiveType, const EPassiveType PassiveType);
};
