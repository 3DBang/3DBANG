#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
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
	void OnRep_Message();

	UFUNCTION()
	void BroadcastChatMessage(const FString& NewMessage, const FString& SenderNickname, const FString& ReciverNickname);

	UFUNCTION()
	void ReceiveMessage(const FString& ChatMessage, const FString& FromNickname, const FString& ReciverNickname);
};