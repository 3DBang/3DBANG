#include "BangGameState.h"
#include "PlayerState/BangPlayerState.h"
#include "PlayerController/BangPlayerController.h"
#include "UI/BangInGameChattingWidget.h"
#include "UI/BangPlayerHUD.H"
#include "Net/UnrealNetwork.h"

ABangGameState::ABangGameState()
{
	bReplicates = true;
}

void ABangGameState::BeginPlay()
{
	Super::BeginPlay();

	
}

void ABangGameState::Server_SendChatMessage_Implementation(uint32 PlayerUniqueID, const FString& Message)
{
	if (ChatMessages.Num() >= 50)
	{
		ChatMessages.RemoveAt(0);
	}

	FString SenderName = TEXT("Unknown");

	for (APlayerState* PS : PlayerArray)
	{
		if (ABangPlayerState* BangPS = Cast<ABangPlayerState>(PS))
		{
			if (BangPS->PlayerInfo.PlayerUniqueID == PlayerUniqueID)
			{
				SenderName = BangPS->PlayerInfo.PlayerName; 
				break;
			}
		}
	}

	// 메시지에 이름 포함해서 추가
	ChatMessages.Add(FChatMessage(PlayerUniqueID, Message, SenderName));

	UE_LOG(LogTemp, Warning, TEXT("[Server] 채팅 수신: UniqueID=%d, Name=%s, Message=%s"), PlayerUniqueID, *SenderName, *Message);

	// 클라이언트에 업데이트
	OnRep_ChatMessages();
}



bool ABangGameState::Server_SendChatMessage_Validate(uint32 PlayerUniqueID, const FString& Message)
{
	return !Message.IsEmpty();
}

/** 클라이언트에서 채팅 메시지가 변경되었을 때 실행 */
void ABangGameState::OnRep_ChatMessages()
{
	UE_LOG(LogTemp, Log, TEXT("[Client] 채팅 메시지 업데이트됨. 현재 총 %d개"), ChatMessages.Num());

	if (ChatMessages.Num() > 0)
	{
		FChatMessage& LatestMessage = ChatMessages.Last();

		if (LatestMessage.PlayerName.IsEmpty())
		{
			for (APlayerState* PS : PlayerArray)
			{
				if (ABangPlayerState* BangPS = Cast<ABangPlayerState>(PS))
				{
					if (BangPS->PlayerInfo.PlayerUniqueID == LatestMessage.PlayerUniqueID)
					{
						LatestMessage.PlayerName = BangPS->PlayerInfo.PlayerName;
						break;
					}
				}
			}
		}

		UE_LOG(LogTemp, Log, TEXT("[Client] 최신 메시지: UniqueID=%d, Sender=%s, Message=%s"),
			LatestMessage.PlayerUniqueID, *LatestMessage.PlayerName, *LatestMessage.Message);

		if (const UWorld* World = GetWorld())
		{
			for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
			{
				if (ABangPlayerController* Controller = Cast<ABangPlayerController>(*It))
				{
					if (ABangPlayerHUD* HUD = Cast<ABangPlayerHUD>(Controller->GetHUD()))
					{
						if (HUD->ChattingWidgetInstance)
						{
							FString FormattedMessage = FString::Printf(TEXT("[%s] %s"), *LatestMessage.PlayerName, *LatestMessage.Message);
							FText NewMessage = FText::FromString(FormattedMessage);

							FSlateColor MessageColor = FSlateColor(FLinearColor::White);
							if (ABangPlayerState* MyPS = Controller->GetPlayerState<ABangPlayerState>())
							{
								if (MyPS->PlayerInfo.PlayerUniqueID == LatestMessage.PlayerUniqueID)
								{
									MessageColor = FSlateColor(FLinearColor::Green);
								}
							}

							HUD->ChattingWidgetInstance->AddMessage(NewMessage, MessageColor);
						}
					}
				}
			}
		}
	}

	OnChatUpdated(); // 블루프린트 델리게이트
}




/** 네트워크 복제 설정 */
void ABangGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABangGameState, ChatMessages);
}
