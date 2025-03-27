#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Data/PlayerInformation.h" 

#include "BangGameState.generated.h"

class UBangCardManager;

UCLASS()
class CH4_TEAM3_BANG_API ABangGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_Message)
	FString Message;

	UPROPERTY(Replicated)
	FString FromPlayerNickname;

	UPROPERTY(Replicated)
	FString ToPlayerNickname;

	UFUNCTION()
	void BroadcastPlayerListToClients();

	UFUNCTION()
	void ReceivePlayerList(const TArray<FPlayerInformation>& InPlayerList) const;

	UFUNCTION()
	void OnRep_PlayerList();

	UPROPERTY(ReplicatedUsing = OnRep_PlayerList)
	TArray<FPlayerInformation> PlayerList;

	UFUNCTION()
	void OnRep_Message();

	UFUNCTION()
	void BroadcastChatMessage(const FString& NewMessage, const FString& SenderNickname, const FString& ReciverNickname);

	UFUNCTION()
	void ReceiveMessage(const FString& ChatMessage, const FString& FromNickname, const FString& ReciverNickname);

	/**
	 * 게임 로그 메시지를 클라이언트들에게 브로드캐스트합니다.
	 *
	 * @param GameLogMessage 브로드캐스트할 게임 로그 메시지
	 */
	UFUNCTION()
	void BroadcastGameLogToClients(const FString& GameLogMessage);

	UFUNCTION()
	void OnRep_GameLog();

	UPROPERTY(ReplicatedUsing = OnRep_GameLog)
	FString CurrentGameLog;

	UFUNCTION()
	void ReceiveGameLog(const FString& GameLogMessage) const;
	virtual void AddPlayerState(APlayerState* NewPlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

};
