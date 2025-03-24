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

	UPROPERTY(ReplicatedUsing = OnRep_PlayerList)
	TArray<FPlayerInformation> PlayerList;

	UFUNCTION()
	void OnRep_PlayerList();

	void BroadcastPlayerListToClients();

	UFUNCTION()
	void OnRep_Message();

	UFUNCTION()
	void BroadcastChatMessage(const FString& NewMessage, const FString& SenderNickname, const FString& ReciverNickname);

	UFUNCTION()
	void ReceiveMessage(const FString& ChatMessage, const FString& FromNickname, const FString& ReciverNickname);

	UFUNCTION()
	void BroadcastGameLogToClients(const FString& GameLogMessage);

	UFUNCTION()
	void OnRep_GameLog();

	UPROPERTY(ReplicatedUsing = OnRep_GameLog)
	FString CurrentGameLog;

};