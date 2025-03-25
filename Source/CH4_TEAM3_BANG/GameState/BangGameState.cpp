
#include "BangGameState.h"
#include "PlayerState/BangPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "PlayerController/BangPlayerController.h"

void ABangGameState::BeginPlay()
{
	Super::BeginPlay();

}

void ABangGameState::BroadcastChatMessage(const FString& NewMessage, const FString& SenderNickname, const FString& ReciverNickname)
{
	Message = NewMessage;
	FromPlayerNickname = SenderNickname;
	ToPlayerNickname = ReciverNickname;

	// 서버에서는 수동 호출 필요
	OnRep_Message();

	ReceiveMessage(Message, FromPlayerNickname, ToPlayerNickname);
}

/**
 * 주어진 채팅 메시지를 모든 플레이어 컨트롤러에게 전달하는 함수입니다.
 *
 * @param ChatMessage 전달할 채팅 메시지입니다.
 * @param FromNickname 메시지를 보낸 플레이어의 닉네임입니다.
 * @param ReciverNickname 메시지의 수신 대상 플레이어 닉네임입니다.
 */
void ABangGameState::ReceiveMessage(const FString& ChatMessage, const FString& FromNickname, const FString& ReciverNickname)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABangPlayerController* BangPlayerControllerC = Cast<ABangPlayerController>(It->Get()))
		{
			BangPlayerControllerC->Client_ReceiveMessage(ChatMessage, FromNickname, ReciverNickname);
		}
	}
}

void ABangGameState::OnRep_Message()
{
	UE_LOG(LogTemp, Log, TEXT("[%s]: %s"), *FromPlayerNickname, *Message);
}

void ABangGameState::BroadcastPlayerListToClients()
{
	PlayerList.Empty();

	for (APlayerState* PS : PlayerArray)
	{
		if (ABangPlayerState* BPS = Cast<ABangPlayerState>(PS))
		{
			PlayerList.Append(BPS->PlayerInfo.Players);
		}
	}

	// 서버에서만 호출 (클라이언트 RPC 호출은 안 함)
	OnRep_PlayerList();

	// 명시적으로 클라이언트 RPC 호출
	ReceivePlayerList(PlayerList);
}

void ABangGameState::ReceivePlayerList(const TArray<FPlayerInformation>& InPlayerList)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABangPlayerController* PC = Cast<ABangPlayerController>(It->Get()))
		{
			PC->Client_UpdatePlayerListUI(InPlayerList);
		}
	}
}

void ABangGameState::OnRep_PlayerList()
{
	UE_LOG(LogTemp, Log, TEXT("PlayerList Replicated: %d players"), PlayerList.Num());
}
void ABangGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABangGameState, Message);
	DOREPLIFETIME(ABangGameState, FromPlayerNickname);
	DOREPLIFETIME(ABangGameState, ToPlayerNickname);
	DOREPLIFETIME(ABangGameState, PlayerList);
	DOREPLIFETIME(ABangGameState, CurrentGameLog);

}

void ABangGameState::BroadcastGameLogToClients(const FString& GameLogMessage)
{
	CurrentGameLog = GameLogMessage;

	// 서버용 로그만 찍음 (클라이언트 RPC 호출하지 않음)
	OnRep_GameLog();

	// 클라이언트에 명시적으로 RPC 호출
	ReceiveGameLog(CurrentGameLog);
}

void ABangGameState::ReceiveGameLog(const FString& GameLogMessage)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABangPlayerController* PC = Cast<ABangPlayerController>(It->Get()))
		{
			PC->Client_UpdateGameLogUI(GameLogMessage);
		}
	}
}

void ABangGameState::OnRep_GameLog()
{
	// 서버에서 로그 확인용
	UE_LOG(LogTemp, Log, TEXT("[GameLog]: %s"), *CurrentGameLog);
}