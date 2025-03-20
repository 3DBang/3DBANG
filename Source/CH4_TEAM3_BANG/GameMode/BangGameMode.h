#pragma once

#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Card/BaseCard/BangCardBase.h"
#include "GameFramework/GameMode.h"
#include "BangGameMode.generated.h"

class UBangCardManager;

USTRUCT(BlueprintType)
struct FPlayerInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FBPUniqueNetId PlayerNetID;

	UPROPERTY()
	FString PlayerName;

	UPROPERTY()
	TObjectPtr<UBangCardBase> JobCard;

	UPROPERTY()
	TObjectPtr<UBangCardBase> CharacterCard;
};

USTRUCT(BlueprintType)
struct FPlayerCollection
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FPlayerInformation> Players;
};

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
	virtual void BeginPlay() override;

	// 플레이어 등록
	UFUNCTION()
	void AddPlayer(const FBPUniqueNetId& PlayerNetID);
	// 플레이어 자리 배치
	UFUNCTION()
	void ArrangeSeats();
	// 게임 시작
	UFUNCTION()
	void StartGame();

private:
	// 카드 매니저
	UPROPERTY()
	TObjectPtr<UBangCardManager> CardManager;

	// 현재 플레이어 목록
	UPROPERTY()
	FPlayerCollection Players;

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
	
	// 게임 턴 이동 (플레이어 변경)
	UFUNCTION()
	void AdvanceGameTurn();
	
	// 플레이어 게임 실행 세부 턴
	UFUNCTION()
	void AdvancePlayerTurn();

	// 플레이어 자리 섞기
	UFUNCTION()
	void ShuffleSeats(FPlayerCollection& ToShufflePlayers) const;
};
