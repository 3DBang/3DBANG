#include "BangGameMode.h"

#include "Card/BangCardManager.h"
#include "Card/CharacterCard/BangCharacterCard.h"
#include "Card/JobCard/BangJobCard.h"

#include "GameState/BangGameState.h"
#include "PlayerState/BangPlayerState.h"
#include "PlayerController/BangPlayerController.h"
#include "../BangCharacter/BangCharacter.h"
#include "Card/ActiveCard/BangActiveCard.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"

ABangGameMode::ABangGameMode()
{
	DefaultPawnClass = ABangCharacter::StaticClass();
	PlayerControllerClass = ABangPlayerController::StaticClass();
    bDelayedStart = true;
}

void ABangGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentPlayerTurnState = EPlayerTurnState::DrawCard;
}

void ABangGameMode::GetPlayerStatesByUniqueID(const int32& UniqueID, FBangPlayerStateCollection& PlayerState_)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABangPlayerController* CastingController = Cast<ABangPlayerController>(It->Get());
		if (CastingController->GetUniqueID() == UniqueID && CastingController)
		{
			PlayerState_.State = CastingController->GetPlayerState<ABangPlayerState>();
		}
	}
}

void ABangGameMode::GetPlayerControllerByUniqueID(const int32& UniqueID, FBangPlayerControllerCollection& PlayerController_)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABangPlayerController* CastingController = Cast<ABangPlayerController>(It->Get());
		if (CastingController->GetUniqueID() == UniqueID && CastingController)
		{
			PlayerController_.Controller = CastingController;
		}
	}
}

void ABangGameMode::GetPlayerCollection(FPlayerCollection& PlayerCollection_) const
{
	if (Players.Players.Num() == 0) return;

	PlayerCollection_ = Players;
}

void ABangGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	PlayerControllers.Add(NewPlayer);
	//게임 시작버튼을 누르면 그때 Player위치 조정함수 사용
	//현재는 테스트용 입니다 
	//SpawnPlayers();
}

void ABangGameMode::AddPlayer(const uint32& UniqueID)
{
	if (CurrentGameState == EGameState::GamePlaying) return;

	FPlayerInformation PlayerInfo;
	PlayerInfo.PlayerUniqueID = UniqueID;
	LobbyPlayers.Players.Add(PlayerInfo);
}

void ABangGameMode::RemovePlayer(const uint32& UniqueID)
{
	if (CurrentGameState == EGameState::GamePlaying) return;

	for (const FPlayerInformation Player : LobbyPlayers.Players)
	{
		if (Player.PlayerUniqueID == UniqueID)
		{
			LobbyPlayers.Players.Remove(Player);
			break;
		}
	}
}

void ABangGameMode::ArrangeSeats()
{
	// 로비 플레이어 등록 후 자리 배치
	for (const FPlayerInformation Player : LobbyPlayers.Players)
	{
		Players.Players.Add(Player);
	}
	
	ShuffleSeats(Players);
}

void ABangGameMode::ShuffleSeats(FPlayerCollection& ToShufflePlayers) const
{
	if (CurrentGameState == EGameState::GamePlaying || ToShufflePlayers.Players.Num() < 4 || ToShufflePlayers.Players.Num() > 7) return;

	const int32 LastIndex = ToShufflePlayers.Players.Num() - 1;
	for (int32 i = LastIndex; i > 0; --i)
	{
		const int32 RandomIndex = FMath::RandRange(0, i);
		ToShufflePlayers.Players.Swap(i, RandomIndex);
	}
}
void ABangGameMode::StartGame()
{
	if (CurrentGameState == EGameState::GamePlaying || !CardManager || Players.Players.Num() < 4 || Players.Players.Num() > 7) return;

	ArrangeSeats();
	
	CurrentGameState = EGameState::GamePlaying;

	// 카드 매니저에 게임 시작 알림
	CardManager->PlayBeginByRole();
	
	// 직업 선택
	TArray<EJobType> JobCards;
	CardManager->GetJobByPlayer(Players.Players.Num(), JobCards);
	FPlayerInformation PlayerInfo;
	
	// 케릭터 카드 분배 & 보안관 먼저 시작
	FCardCollection CharacterCards;
	for (int16 i = 0; i < Players.Players.Num(); i++)
	{
		Players.Players[i].JobCardType = JobCards[i];
		Players.Players[i].CharacterCardType = CardManager->GetCharacterCard();
		
		if (JobCards[i] == EJobType::Officer)
		{
			CurrentPlayerTurn = Players.Players[i].PlayerName;
			PlayerIndex = i;
		}
	}

	AdvanceGameTurn();
}

void ABangGameMode::LooseCardFromHanded(const ESymbolType SymbolType, const int32 SymbolNumber, const bool IsToUsed) const
{
	FSingleCard SingleCard;
	CardManager->GetCardBySymbolAndNumber(SymbolType, SymbolNumber, true, SingleCard);

	if (IsToUsed)
	{
		CardManager->ReorderUsedCards(SingleCard);
	}
	else
	{
		CardManager->ReorderAvailCards(SingleCard);
	}
}

void ABangGameMode::AdvanceGameTurn()
{
	if (CurrentGameState == EGameState::GameOver || !CardManager) return;

	if (CurrentPlayerTurnState == EPlayerTurnState::DrawCard) // 현재 턴인 플레이어가 카드뽑기 단계 일때
	{
		switch (Players.Players[PlayerIndex].CharacterCardType)
		{
		case ECharacterType::PedroRamirez:
			{
				FCardCollection DrawCards;
				CardManager->HandCards(2, DrawCards);
				// PlayerState 한테 넘겨줌
				FBangPlayerStateCollection CurrentPlayerState;
				GetPlayerStatesByUniqueID(Players.Players[PlayerIndex].PlayerUniqueID, CurrentPlayerState);
				//CurrentPlayerState->

				// 플레이어 컨트롤러한테 턴 시작 알림
				FBangPlayerControllerCollection CurrentPlayerController;
				GetPlayerControllerByUniqueID(Players.Players[PlayerIndex].PlayerUniqueID, CurrentPlayerController);
				//CurrentPlayerController.Controller->Server_PlayerTurn();
				break;
			}
		case ECharacterType::BlackJack:
			{
				// 두번째로 뽑은 카드를 모든 플레이어에게 공개, 카드의 심벌이 하트나 다이아면 한장을 더 드로우
				FCardCollection CardList;
				CardManager->HandCards(2, CardList);
				if (CardList.CardList[1].Card->SymbolType == ESymbolType::Heart || CardList.CardList[1].Card->SymbolType == ESymbolType::Diamond)
				{
					CardManager->HandCards(1, CardList);
				}
				// PlayerState한테 CardList 보내기
				break;
			}
		case ECharacterType::KitCarlson:
			{
				// 카드 더미위에 3장을 보고 그중에서 두개를 가져가고 한장은 다시 뽑는 카드더미 위에 올려둔다.
				FCardCollection CardList;
				CardManager->HandCards(3, CardList);
				// PlayerState한테 3보낸다. 
				break;
			}
		default:
			{
				FCardCollection CardList;
				CardManager->HandCards(2, CardList);
				// PlayerState 플레이어 스테이트한테 카드 전달
				break;
			}
		}	
		// 플레이어 한테 응답 받고 아래 로직 실행
		CurrentPlayerTurnState = EPlayerTurnState::UseCard;
		AdvanceGameTurn();
	}
	else if (CurrentPlayerTurnState == EPlayerTurnState::UseCard)
	{
		switch (Players.Players[PlayerIndex].CharacterCardType)
		{
		case ECharacterType::PaulRegret:
			break;
		case ECharacterType::BartCassidy:
			break;
		case ECharacterType::CalamityJanet:
			break;
		case ECharacterType::Jourdonnais:
			{
				// TODO::뱅의 표적이 될때마다 카드펼치기를 할 수 있으며 하트가 나오면 총알이 빗나감
				break;
			}
		case ECharacterType::PedroRamirez:
			break;
		case ECharacterType::BlackJack:
			break;
		case ECharacterType::JesseJones:
			{
				// TODO::카드 가저오기 단계에서 첫번쨰 카드를 다른사람에게서 가져올 수 있다.
				break;
			}
		case ECharacterType::SuzyLafayette:
			break;
		case ECharacterType::SidKetchum:
			break;
		case ECharacterType::LuckyDuke:
			break;
		case ECharacterType::SlabTheKiller:
			{
				// TODO::본인이 쏜 뱅은 다른사람이 빗나감 두장으로 막아야함 PS
				break;
			}
		case ECharacterType::ElGringo:
			break;
		case ECharacterType::RoseDoolan:
			{
				// TODO::다른사람을 볼때 거리 1이 가까워 진다. PS
				break;
			}
		case ECharacterType::WillyTheKid:
			break;
		case ECharacterType::KitCarlson:
			break;
		case ECharacterType::VultureSam:
			break;
		case ECharacterType::None:
			break;
		}	
	}
	else if (CurrentPlayerTurnState == EPlayerTurnState::LooseCard)
	{
		AdvancePlayerTurn();
	}
}

void ABangGameMode::EndTurn(const uint32 UniqueID, ECharacterType PlayerCharacter)
{
	CurrentPlayerTurnState = EPlayerTurnState::LooseCard;
	AdvanceGameTurn();
}

void ABangGameMode::LooseCard(const FCardCollection CardList)
{
	if (CardList.CardList.Num() == 0) return;

	for (const FSingleCard Card : CardList.CardList)
	{
		CardManager->ReorderUsedCards(Card);
	}
}

void ABangGameMode::LooseSidKetchumCard(const FCardCollection CardList)
{
	if (CardList.CardList.Num() != 2) return;

	for (const FSingleCard CardType : CardList.CardList)
	{
		CardManager->ReorderUsedCards(CardType);
	}
}

void ABangGameMode::PlayerDead(const uint32 UniqueID,
		const ECharacterType PlayerCharacter,
		const EJobType JobType,
		const FCardCollection CardList)
{
	// 플레이어중에 벌처셈 카드가 있으면 그상대에게 카드를 다 줘야한다.
	for (FPlayerInformation Player : Players.Players)
	{
		if (Player.CharacterCardType == ECharacterType::VultureSam)
		{
			// 상대에게 카드 주기
		}
		else
		{
			// 카드 사용덱으로 이동
		}	
	}

	// 플레이어 죽었을 경우 게임 종료조건 체크
	switch (JobType)
	{
	case EJobType::Officer:
		// 게임오버 -> 무법자가 살아있으면 무법자 승리
		//		   -> 부관이랑, 무법자가 죽어있으면 배신자 승리
		{
			for (const FPlayerInformation Player : Players.Players)
			{
				if (Player.JobCardType == EJobType::Betrayer)
				{
					
				}
			}
			break;
		}
	case EJobType::SubOfficer:
		{
			if (Players.Players[PlayerIndex].JobCardType == EJobType::Officer) // 보안관이 죽였을 경우
			{
				// PS 에 정보 념겨줌 (GM -> PS -> PC -> GM)
			}
			break;
		}
	case EJobType::Outlaw:
		// 무법자를 죽인사람한테 카드 3장 사용하지 않은 덱에서 전달
		{
			// 카드 3장을 뽑아서 PS한테 전달
			FCardCollection NewCardList;
			CardManager->HandCards(3, NewCardList);
			break;
		}
	case EJobType::Betrayer:
		break;
	}

	// 플레이어 제거
	for (uint16 i = 0; i < Players.Players.Num(); i++)
	{
		if (Players.Players[i].PlayerUniqueID == UniqueID)
		{
			Players.Players.RemoveAt(i);
			break;
		}
	}
}

void ABangGameMode::UseCard(
	const uint32 UniqueID,
	const FPlayerCardSymbol& Card, 
	const EActiveType ActiveType,
	const EPassiveType PassiveType,
	const ECharacterType CharacterType,
	const uint32 ToUniqueID,
	const ECharacterType ToCharacterType) const
{
	if (UniqueID == 0) return;
	bool bIsAbleToUse = false;
	// 플레이어가 사용하는 카드가 타당한지 검사 후 가능여부 리턴

	if (CurrentPlayerTurnState == EPlayerTurnState::UseCard)
	{
		switch (Players.Players[PlayerIndex].CharacterCardType)
		{
		case ECharacterType::Jourdonnais:
			{
				// 뱅의 표적이 될때마다 카드펼치기를 할 수 있으며 하트가 나오면 총알이 빗나감
				break;
			}
		case ECharacterType::JesseJones:
			{
				// 카드 가저오기 단계에서 첫번쨰 카드를 다른사람에게서 가져올 수 있다.
				break;
			}
		case ECharacterType::SlabTheKiller:
			{
				// 본인이 쏜 뱅은 다른사람이 빗나감 두장으로 막아야함 PS
				break;
			}
		case ECharacterType::RoseDoolan:
			{
				// 다른사람을 볼때 거리 1이 가까워 진다. PS
				break;
			}
		default:
			break;
		}	
		// 플레이어 한테 응답 받고 아래 로직 실행
	}

	//CastingController->Server_UseCardReturn(bIsAbleToUse);
}

void ABangGameMode::UsePanicCard(const EActiveType ActiveType, const EPassiveType PassiveType)
{
	
}

void ABangGameMode::UseCatBalouCard(const EActiveType ActiveType, const EPassiveType PassiveType)
{
	
}

/*void ABangGameMode::SetUserHP()
{
	if (ABangPlayerController* PlayerController = Cast<ABangPlayerController>(PlayerControllers[1]))
	{
		PlayerController->SetInitializeHP(5);
	}
}*/

void ABangGameMode::AdvancePlayerTurn()
{
	PlayerIndex++;
	PlayerIndex = PlayerIndex % Players.Players.Num();
	CurrentPlayerTurn = Players.Players[PlayerIndex].PlayerName;
	CurrentPlayerTurnState = EPlayerTurnState::DrawCard;
	AdvanceGameTurn();
}

void ABangGameMode::SpawnPlayers()
{
    TObjectPtr<APlayerStart> BasePlayerStart = ChooseStartLocation();
    if (!BasePlayerStart)
    {
        UE_LOG(LogTemp, Warning, TEXT("No PlayerStart found to use as center."));
        return;
    }
    
    UE_LOG(LogTemp, Error, TEXT("Player Start with Flagged is  : %s"), *BasePlayerStart->GetActorLocation().ToString());


    FVector Center = BasePlayerStart->GetActorLocation();

    int32 PlayersNum = PlayerControllers.Num();
    UE_LOG(LogTemp, Error, TEXT("Player Num is %d"),PlayersNum);
    if (PlayersNum <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No PlayerControllers available for spawning."));
        return;
    }

    for (int32 i = 0; i < PlayersNum; i++)
    {
        float Radian = (2 * PI / PlayersNum) * i;
        FVector Offset(FMath::Cos(Radian) * Radius, FMath::Sin(Radian) * Radius, 0.f);
        FVector SpawnLocation = Center + Offset;
        FRotator SpawnRotation = (Center - SpawnLocation).Rotation();
        FRotator DefaultRotation = DefaultPawnClass->GetDefaultObject<ABangCharacter>()->GetActorRotation();
        SpawnRotation -= DefaultRotation;

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = PlayerControllers[i];
        SpawnParams.Instigator = PlayerControllers[i]->GetPawn();

        ABangCharacter* Player = GetWorld()->SpawnActor<ABangCharacter>(DefaultPawnClass, SpawnLocation, SpawnRotation, SpawnParams);
        if (Player)
        {
            PlayerControllers[i]->Possess(Player);
            UE_LOG(LogTemp, Error, TEXT("플레이어컨트롤러가 폰에 빙의했습니다."));
           
            if (ABangPlayerController* PlayerController = Cast<ABangPlayerController>(PlayerControllers[i]))
            {
                PlayerController->Client_SetControllerRotation(SpawnRotation);
            }
           //TODO : Suffle

            UE_LOG(LogTemp, Error, TEXT("Player Controller is %s"), *PlayerControllers[i]->GetName());

            UE_LOG(LogTemp, Error, TEXT("Spawn Location is : %s"), *SpawnLocation.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to spawn Pawn for player index %d"), i);
        }
    }
}

APlayerStart* ABangGameMode::ChooseStartLocation() const
{
    TArray<AActor*> AllPlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), AllPlayerStarts);

    APlayerStart* FlaggedStarts;
    for (AActor* Actor : AllPlayerStarts)
    {
        APlayerStart* Start = Cast<APlayerStart>(Actor);
        if (Start && Start->ActorHasTag(FName("Flagged")))
        {
            FlaggedStarts = Start;
            return FlaggedStarts;
        }
    }
    return nullptr;
}

void ABangGameMode::SpawnPlayerBlue()
{
    SpawnPlayers();
}
