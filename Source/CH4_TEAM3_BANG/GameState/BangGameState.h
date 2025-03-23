#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BangGameState.generated.h"

class UBangCardManager;

USTRUCT(BlueprintType)
struct FChatMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 PlayerUniqueID;  // 플레이어의 고유 ID

	UPROPERTY()
	FString PlayerName;
	UPROPERTY(BlueprintReadOnly)
	FString Message;  // 채팅 내용

	FChatMessage() {}

	FChatMessage(uint32 InPlayerUniqueID, const FString& InMessage, const FString& PlayerName)
		: PlayerUniqueID(InPlayerUniqueID), Message(InMessage), PlayerName(PlayerName) {
	}
};

UCLASS()
class CH4_TEAM3_BANG_API ABangGameState : public AGameState
{
	GENERATED_BODY()
public:

	ABangGameState();

	virtual void BeginPlay() override;

	/** 현재 채팅 메시지 목록 (모든 클라이언트에 동기화됨) */
	UPROPERTY(ReplicatedUsing = OnRep_ChatMessages, BlueprintReadOnly)
	TArray<FChatMessage> ChatMessages;

	/** 채팅 메시지가 변경되면 클라이언트에서 실행됨 */
	UFUNCTION()
	void OnRep_ChatMessages();

	/** 블루프린트에서 UI 업데이트를 위한 이벤트 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Chat")
	void OnChatUpdated();

	/** 서버에서 채팅 메시지를 저장하고 클라이언트에 전송 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendChatMessage(uint32 PlayerUniqueID, const FString& Message);


protected:
	/** 네트워크 복제 설정 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
