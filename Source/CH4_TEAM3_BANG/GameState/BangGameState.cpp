#include "BangGameState.h"

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

void ABangGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABangGameState, Message);
	DOREPLIFETIME(ABangGameState, FromPlayerNickname);
	DOREPLIFETIME(ABangGameState, ToPlayerNickname);
}