#include "BangGameMode.h"
#include "EngineUtils.h"

#include "Card/BangCardManager.h"
#include "Card/JobCard/BangJobCard.h"

#include "GameState/BangGameState.h"
#include "PlayerState/BangPlayerState.h"
#include "PlayerController/BangPlayerController.h"
#include "../BangCharacter/BangCharacter.h"
#include "Card/ActiveCard/BangActiveCard.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Instance/BangGameInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Card/BangCardActor.h"
#include "Card/BangCardTableSpawner.h"

ABangGameMode::ABangGameMode()
{
	DefaultPawnClass = ABangCharacter::StaticClass();
	PlayerControllerClass = ABangPlayerController::StaticClass();
    bDelayedStart = true;

	CurrentGameState = EGameState::GameOver;
	CurrentPlayerTurnState = EPlayerTurnState::DrawCard;
}

void ABangGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 카드매니저 가져오기
	if (const TObjectPtr<UBangGameInstance> BangGameInstance = Cast<UBangGameInstance>(GetGameInstance()))
	{
		FCardManagerInstance OutCardManager;
		BangGameInstance->GetCardManager(OutCardManager);
		CardManager = OutCardManager.CardManager;
		UE_LOG(LogTemp, Warning, TEXT("[BangGameMode::BeginPlay] CardManager Loaded"));
		
		// 카드 매니저 초기 셋팅 (GameMode에서만 진행)
		CardManager->PlayBeginByRole();
	}

	for (TActorIterator<ABangCardTableSpawner> It(GetWorld()); It; ++It)
	{
		if (ABangCardTableSpawner* Table = *It)
		{
			Table->CardManager = CardManager;
			Table->SpawnDeckCards();           // 중앙 덱 시각화
			Table->SpawnHandCards();  // 여기도 호출해줘야 스폰됨!

			break;
		}
	}
}

void ABangGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("[BangGameMode::PostLogin] Player Login"));
	
	if (const FString MapName = GetWorld()->GetMapName(); MapName.Contains("StageMap")
		|| MapName.Contains("Hwang")
		|| MapName.Contains("Bin_TestMap1"))
	{
		if (TObjectPtr<ABangPlayerController> BangPlayerController = Cast<ABangPlayerController>(NewPlayer))
		{
			FBangSinglePlayerController Controller;
			Controller.Controller = BangPlayerController;
			BangPlayerControllers.Add(BangPlayerController);
			BangPlayerController->Init();
			
			AddLobbyPlayer(PlayerUniqueIndex++, BangPlayerController->PlayerNickname, Controller);
			SendGameLog(FString::Printf(TEXT("%s님이 입장했습니다."), *BangPlayerController->PlayerNickname));
		}
	}
	//게임 시작버튼을 누르면 그때 Player위치 조정함수 사용
	//현재는 테스트용 입니다 
	//SpawnPlayers();
}

void ABangGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (TObjectPtr<ABangPlayerController> BangPlayerController = Cast<ABangPlayerController>(Exiting))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BangGameMode::Logout] Player %s 로그아웃"), *BangPlayerController->PlayerNickname);
		BangPlayerControllers.Remove(BangPlayerController);

		const uint32 UniqueID = BangPlayerController->PlayerUniqueID;
		LobbyPlayers.RemovePlayer(UniqueID);
		ForceUpdate_RemovePlayer(UniqueID);
		SendGameLog(FString::Printf(TEXT("%s님이 퇴장하였습니다."), *BangPlayerController->PlayerNickname));
	}
}

void ABangGameMode::GetPlayerStatesByUniqueID(const int32& UniqueID, FBangSinglePlayerState& PlayerState_)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (const TObjectPtr<ABangPlayerController> CastingController = Cast<ABangPlayerController>(It->Get()))
		{
			if (const TObjectPtr<ABangPlayerState> PlayerState = CastingController->GetPlayerState<ABangPlayerState>())
			{
				if (PlayerState->PlayerUniqueID == UniqueID && CastingController)
				{
					PlayerState_.State = CastingController->GetPlayerState<ABangPlayerState>();
				}
			}
		}
	}
}

void ABangGameMode::GetPlayerControllerByUniqueID(const int32& UniqueID, FBangSinglePlayerController& PlayerController_)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (const TObjectPtr<ABangPlayerController> CastingController = Cast<ABangPlayerController>(It->Get()))
		{
			if (const TObjectPtr<ABangPlayerState> PlayerState = CastingController->GetPlayerState<ABangPlayerState>())
			{
				if (PlayerState->PlayerUniqueID == UniqueID && CastingController)
				{
					PlayerController_.Controller = CastingController;
				}
			}
		}
	}
}

void ABangGameMode::GetPlayerCollection(FPlayerCollection& OutPlayerCollection) const
{
	if (Players.Players.Num() == 0) return;

	OutPlayerCollection = Players;
}

void ABangGameMode::AddLobbyPlayer(const uint32& UniqueID, const FString& PlayerNickName, const FBangSinglePlayerController& PlayerController)
{
	if (CurrentGameState == EGameState::GamePlaying) return;

	if (LobbyPlayers.GetPlayerInformation(UniqueID) != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BangGameMode::AddLobbyPlayer] Duplicate Player ID: %u - Not Adding"), UniqueID);
		return;
	}

	FPlayerInformation PlayerInfo;
	PlayerInfo.PlayerUniqueID = UniqueID;
	PlayerInfo.PlayerName = PlayerController.Controller->PlayerNickname;
	
	UE_LOG(LogTemp, Warning, TEXT("[BangGameMode::AddLobbyPlayer] Player ID: %u nickname: %s"), UniqueID, *PlayerNickName);

	// PS에 전달
	if (TObjectPtr<ABangPlayerState> BangPlayerState = PlayerController.Controller->GetPlayerState<ABangPlayerState>())
	{
		//BangPlayerState->Client_SetUniqueId(UniqueID);
		BangPlayerState->PlayerUniqueID = UniqueID;
		UE_LOG(LogTemp, Warning, TEXT("[BangGameMode::SetPlayerUniqueID] PlayerUniqueIndex: %d"), UniqueID);
	}
	
	LobbyPlayers.Players.Add(PlayerInfo);
}

// 로비 플레이어 삭제
void ABangGameMode::RemoveLobbyPlayer(const uint32& UniqueID)
{
	for (const FPlayerInformation Player : LobbyPlayers.Players)
	{
		if (Player.PlayerUniqueID == UniqueID)
		{
			LobbyPlayers.Players.Remove(Player);
			break;
		}
	}
}

// 플레이어 삭제
void ABangGameMode::ForceUpdate_RemovePlayer(const uint32& UniqueID)
{
	if (CurrentGameState == EGameState::GamePlaying) return;

	for (const FPlayerInformation Player : Players.Players)
	{
		if (Player.PlayerUniqueID == UniqueID)
		{
			Players.Players.Remove(Player);
			break;
		}
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const TObjectPtr<ABangPlayerController> CastingController = Cast<ABangPlayerController>(It->Get());
		const TObjectPtr<ABangPlayerState> BangPlayerState = CastingController->GetPlayerState<ABangPlayerState>();

		if (GetNetMode() == NM_ListenServer)
		{
			BangPlayerState->HandlePlayerInfoUpdated();
		}
		
		BangPlayerState->PlayerInfo.RemovePlayer(UniqueID);
		BangPlayerState->ForceNetUpdate();
	}
}

// 플레이어 자리 배치
void ABangGameMode::ArrangeSeats()
{
	// 로비 플레이어 등록 후 자리 배치
	for (const FPlayerInformation Player : LobbyPlayers.Players)
	{
		Players.Players.Add(Player);
	}
	
	ShuffleSeats(Players);

	// 한점 기준으로 플레이어 나열 Players
	SpawnPlayers();
}

void ABangGameMode::ShuffleSeats(FPlayerCollection& ToShufflePlayers)
{
	if (CurrentGameState == EGameState::GamePlaying || ToShufflePlayers.Players.Num() < 4 || ToShufflePlayers.Players.Num() > 7) return;

	const int32 LastIndex = ToShufflePlayers.Players.Num() - 1;
	for (int32 i = LastIndex; i > 0; --i)
	{
		const int32 RandomIndex = FMath::RandRange(0, i);
		BangPlayerControllers.Swap(i, RandomIndex);
		ToShufflePlayers.Players.Swap(i, RandomIndex);
	}
}

void ABangGameMode::StartTest()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Test"));
	SpawnPlayers();
	SendGameLog(FString::Printf(TEXT("게임이 시작되었습니다.")));
	Players.Players.Empty();

	// 기존 코드 계속 진행
	for (int i = 0; i < LobbyPlayers.Players.Num(); ++i)
	{
		FPlayerInformation PlayerInformation;
		FString PlayerName = FString::Printf(TEXT("Player[%d]"), LobbyPlayers.Players[i].PlayerUniqueID);
		PlayerInformation.PlayerName = PlayerName;
		PlayerInformation.PlayerUniqueID = LobbyPlayers.Players[i].PlayerUniqueID;
		if (i == 0) // 보안관
		{
			PlayerInformation.JobCardType = EJobType::Officer;
			PlayerInformation.CharacterCardType = ECharacterType::ElGringo;
		}
		else
		{
			PlayerInformation.JobCardType = EJobType::Betrayer;
			PlayerInformation.CharacterCardType = ECharacterType::BartCassidy;

		}

		PlayerInformation.MaxHealth = 4;
		PlayerInformation.CurrentHealth = 4;
		PlayerInformation.Range = 1;
		PlayerInformation.CharacterRange = 0;
		PlayerInformation.bIsMyTurn = false;

		Players.Players.Add(PlayerInformation);
	}

	CurrentGameState = EGameState::GamePlaying;

	UE_LOG(LogTemp, Warning, TEXT("Players created: %d"), Players.Players.Num());

	TArray<EJobType> JobCards;
	JobCards.Add(EJobType::Officer);
	JobCards.Add(EJobType::SubOfficer);

	FCardCollection CharacterCards;
	for (int16 i = 0; i < Players.Players.Num(); i++)
	{
		Players.Players[i].JobCardType = JobCards[i];
		Players.Players[i].CharacterCardType = CardManager->GetCharacterCard();
		if (Players.Players[i].CharacterCardType == ECharacterType::RoseDoolan)
		{
			Players.Players[i].Range++;
		}
		else if (Players.Players[i].CharacterCardType == ECharacterType::PaulRegret)
		{
			Players.Players[i].CharacterRange++;
		}
		Players.Players[i].MaxHealth = CardManager->GetHealthByCharacteType(CardManager->GetCharacterCard());

		// 최초 카드 분배
		int16 Health = Players.Players[i].MaxHealth;
		FCardCollection Cards;
		CardManager->HandCards(Health, Cards);
		Players.Players[i].MyCards.AddCardCollectionToPlayerCards(Cards);
		if (JobCards[i] == EJobType::Officer)
		{
			CurrentTurnPlayerUniqeID = Players.Players[i].PlayerUniqueID;
			Players.Players[i].bIsMyTurn = true; 
			PlayerIndex = i;
		}
	}

	for (FPlayerInformation Player : Players.Players)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerName: %s"), *Player.PlayerName);
		UE_LOG(LogTemp, Warning, TEXT("PlayerUniqueID: %d"), Player.PlayerUniqueID);
		UE_LOG(LogTemp, Warning, TEXT("JobCardType: %d"), Player.JobCardType);
		UE_LOG(LogTemp, Warning, TEXT("MaxHealth: %d"), Player.MaxHealth);
		UE_LOG(LogTemp, Warning, TEXT("CharacterCardType: %d"), Player.CharacterCardType);
	}

	UE_LOG(LogTemp, Warning, TEXT("CurrentPlayerTurn: %d"), CurrentTurnPlayerUniqeID);

	// PS 동기화
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (TObjectPtr<ABangPlayerController> CastingController = Cast<ABangPlayerController>(It->Get()))
		{
			if (const TObjectPtr<ABangPlayerState> BangPlayerState = CastingController->GetPlayerState<ABangPlayerState>())
			{
				// 최초 등록 동기화
				BangPlayerState->PlayerInfo = Players;
				if (GetNetMode() == NM_ListenServer)
				{
					BangPlayerState->HandlePlayerInfoUpdated();
				}
		
				BangPlayerState->ForceNetUpdate();
			}
		}
	}

	if (Players.Players.Num() > 0)
	{
		AdvanceGameTurn();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("플레이어가 생성되지 않았습니다. AdvanceGameTurn을 건너뜁니다."));
	}

	// 사용된 카드 테스트 코드 
	for (int i = 0; i < 3; ++i)
	{
		FCardCollection Draw;
		CardManager->HandCards(1, Draw);

		if (Draw.CardList.Num() > 0)
		{
			FSingleCard Card = Draw.CardList[0];
			CardManager->ReorderUsedCards(Card);
		}
	}

	for (TActorIterator<ABangCardTableSpawner> It(GetWorld()); It; ++It)
	{
		if (ABangCardTableSpawner* Table = *It)
		{
			Table->SpawnUsedCards();
			Table->SpawnHandCards(); 

			break;
		}
	}
}

// 시작할때 컨트롤러에서 플레이어 아이디랑 플레이어를 PS에 갱신해준다.
void ABangGameMode::ForceUpdate_StartGame_Real()
{
	SendGameLog(FString::Printf(TEXT("게임 시작! 참여인원 %d"), LobbyPlayers.Players.Num()));
	UE_LOG(LogTemp, Warning, TEXT("StartGame [%d]"), LobbyPlayers.Players.Num());
	if (!CardManager) return;
	if (CurrentGameState == EGameState::GamePlaying || !CardManager) return;
	if (LobbyPlayers.Players.Num() < 4 || LobbyPlayers.Players.Num() > 7) return;

	CurrentGameState = EGameState::GamePlaying;
	
	ArrangeSeats();
	
	// 직업 선택
	TArray<EJobType> JobCards;
	CardManager->GetJobByPlayer(Players.Players.Num(), JobCards);
	
	// 케릭터 카드 분배 & 보안관 먼저 시작
	FCardCollection CharacterCards;
	for (int16 i = 0; i < Players.Players.Num(); i++)
	{
		Players.Players[i].JobCardType = JobCards[i];
		Players.Players[i].CharacterCardType = CardManager->GetCharacterCard();
		// 로즈 둘란 사거리 증가
		if (Players.Players[i].CharacterCardType == ECharacterType::RoseDoolan)
		{
			Players.Players[i].Range++;
		}
		// 폴리그렛 상대사거리 증가
		else if (Players.Players[i].CharacterCardType == ECharacterType::PaulRegret)
		{
			Players.Players[i].CharacterRange++;
		}
		Players.Players[i].MaxHealth = CardManager->GetHealthByCharacteType(CardManager->GetCharacterCard());

		// 최초 카드 분배
		int16 Health = Players.Players[i].MaxHealth;
		FCardCollection Cards;
		CardManager->HandCards(Health, Cards);
		Players.Players[i].MyCards.AddCardCollectionToPlayerCards(Cards);
		
		if (JobCards[i] == EJobType::Officer)
		{
			CurrentTurnPlayerUniqeID = Players.Players[i].PlayerUniqueID;
			Players.Players[i].bIsMyTurn = true;
			PlayerIndex = i;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("CurrentPlayerTurn: %d"), CurrentTurnPlayerUniqeID);

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (TObjectPtr<ABangPlayerController> CastingController = Cast<ABangPlayerController>(It->Get()))
		{
			if (const TObjectPtr<ABangPlayerState> BangPlayerState = CastingController->GetPlayerState<ABangPlayerState>())
			{
				if (GetNetMode() == NM_ListenServer)
				{
					BangPlayerState->HandlePlayerInfoUpdated();
				}
		
				// 최초 등록 동기화
				BangPlayerState->PlayerInfo = Players;
				BangPlayerState->ForceNetUpdate();
			}
		}
		
		//플레이어 카드리스트 초기화
		//CastingController->Client_UpdateCardList(); 
		//UE_LOG(LogTemp, Log, TEXT("PlayerState synced for controller: %s"), *CastingController->GetName());
	}

	AdvanceGameTurn();
}

void ABangGameMode::ForceUpdate_LooseCardFromHanded(const int32 FromUniqueID, const ESymbolType SymbolType, const int32 SymbolNumber, const EDeckType DeckType)
{
	if (!CardManager) return;
	FSingleCard SingleCard;
	CardManager->GetCardBySymbolAndNumber(SymbolType, SymbolNumber, DeckType, SingleCard);

	switch (DeckType)
	{
	case EDeckType::HandedCard: break;
	case EDeckType::UsedCards:
		{
			CardManager->ReorderUsedCards(SingleCard); // 건내준 카드를 다시 사용된 카드 덱에 넣는다
			break;
		}
	case EDeckType::AvailCards:
		{
			CardManager->ReorderAvailCards(SingleCard); // 건내준 카드를 다시 사용가능한 카드 덱에 넣는다
			break;
		}
	}

	FBangSinglePlayerState PlayerState;
	GetPlayerStatesByUniqueID(FromUniqueID, PlayerState);
	PlayerState.State->PlayerInfo.GetPlayerInformation(FromUniqueID)->MyCards.RemoveCard(SingleCard.Card->SymbolType, SingleCard.Card->SymbolNumber);
	PlayerState.State->ForceNetUpdate();
}

void ABangGameMode::AdvanceGameTurn()
{
	if (CurrentGameState == EGameState::GameOver || !CardManager) return;

	if (CurrentPlayerTurnState == EPlayerTurnState::DrawCard) // 현재 턴인 플레이어가 카드뽑기 단계 일때
	{
		// 트랩카드 처리 (다이너마이트, 감옥)	✨
		CheckTrapCard();

		// 액터 이동 가능 처리
		ReSpawnPlayerAtTurn();

		// 카드 뽑기
		FCardCollection DrawCards;
		switch (Players.Players[PlayerIndex].CharacterCardType)
		{
		case ECharacterType::PedroRamirez:
			{
				CardManager->HandCards(2, DrawCards);
				
				break;
			}
		case ECharacterType::BlackJack:
			{
				// 두번째로 뽑은 카드를 모든 플레이어에게 공개, 카드의 심벌이 하트나 다이아면 한장을 더 드로우
				CardManager->HandCards(2, DrawCards);
				if (DrawCards.CardList[1].Card->SymbolType == ESymbolType::Heart || DrawCards.CardList[1].Card->SymbolType == ESymbolType::Diamond)
				{
					CardManager->HandCards(1, DrawCards);
				}
				break;
			}
		case ECharacterType::KitCarlson:
			{
				// 카드 더미위에 3장을 보고 그중에서 두개를 가져가고 한장은 다시 뽑는 카드더미 위에 올려둔다.
				CardManager->HandCards(3, DrawCards);

				// PC에 카드 뭐 선택할껀지 요청 CurrentTurnPlayerUniqeID
				FBangSinglePlayerController PlayerController;
				GetPlayerControllerByUniqueID(CurrentTurnPlayerUniqeID, PlayerController);

				FPlayerCardCollection PlayerCards;
				PlayerCards.AddCardCollectionToPlayerCards(DrawCards);
				
				return;
			}
		default:
			{
				CardManager->HandCards(2, DrawCards);
				break;
			}
		}
		
		FBangSinglePlayerState CurrentPlayerState;
		GetPlayerStatesByUniqueID(Players.Players[PlayerIndex].PlayerUniqueID, CurrentPlayerState);
		
		FPlayerCardCollection DrawSymbolCollections;
		
		SendGameLog(FString::Printf(TEXT("플레이어 %s 카드 드로우턴"), *Players.Players[PlayerIndex].PlayerName));
				
		for (FSingleCard CardList : DrawCards.CardList)
		{
			FPlayerCardSymbol DrawSymbol;
			DrawSymbol.SymbolNumber = CardList.Card->SymbolNumber;
			DrawSymbol.SymbolType = CardList.Card->SymbolType;
			DrawSymbolCollections.PlayerCards.Add(DrawSymbol);
		}
		
		CurrentPlayerState.State->Client_StartTurn(CurrentTurnPlayerUniqeID, DrawSymbolCollections);
		
		CurrentPlayerTurnState = EPlayerTurnState::UseCard;
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
		ForceUpdate_AdvancePlayerTurn();
	}
}

void ABangGameMode::CheckTrapCard()
{
	for (FPlayerCardSymbol PlayerCard : Players.Players[PlayerIndex].TrapCards.PlayerCards)
	{
		FSingleCard OutFoundCard;
		CardManager->GetCardBySymbolAndNumberFromDataAsset(PlayerCard.SymbolType, PlayerCard.SymbolNumber, OutFoundCard);

		if (TObjectPtr<UBangActiveCard> BangActiveCard = Cast<UBangActiveCard>(OutFoundCard.Card))
		{
			if (BangActiveCard->ActiveType == EActiveType::Jail)
			{
				FCardCollection OutCards;
				CardManager->CheckCardSymbolFromAvailCards(1, OutCards);

				FText OutCardsCardName = OutCards.CardList[0].Card->CardName;
				FString OutCardsSymbolType = GetEnumToString<ESymbolType>(OutCards.CardList[0].Card->SymbolType);
				SendGameLog(FString::Printf(TEXT("감옥 카드 뽑기 : 뽑은 카드 %s , 카드 심볼 : %s"), *OutCardsCardName.ToString(), *OutCardsSymbolType));
				
				if (OutCards.CardList[0].Card->SymbolType != ESymbolType::Heart)
				{
					ForceUpdate_AdvancePlayerTurn();
					return;
				}
			}
			else if (BangActiveCard->ActiveType == EActiveType::Dynamite)
			{
				FCardCollection OutCards;
				CardManager->CheckCardSymbolFromAvailCards(1, OutCards);

				FText OutCardsCardName = OutCards.CardList[0].Card->CardName;
				FString OutCardsSymbolType = GetEnumToString<ESymbolType>(OutCards.CardList[0].Card->SymbolType);
				SendGameLog(FString::Printf(TEXT("다이너마이트 카드 뽑기 : 뽑은 카드 %s , 카드 심볼 : %s"), *OutCardsCardName.ToString(), *OutCardsSymbolType));
				
				if (OutCards.CardList[0].Card->SymbolType == ESymbolType::Spade
					&& (OutCards.CardList[1].Card->SymbolNumber >= 2 || OutCards.CardList[1].Card->SymbolNumber <= 9))
				{
					if (Players.Players[PlayerIndex].CurrentHealth < 4)
					{
						FPlayerCardCollection CardList;
						CardList.PlayerCards.Append(Players.Players[PlayerIndex].MyCards.PlayerCards);
						CardList.PlayerCards.Append(Players.Players[PlayerIndex].EquippedCards.PlayerCards);
						CardList.PlayerCards.Append(Players.Players[PlayerIndex].TrapCards.PlayerCards);
							
						PlayerDead(CurrentTurnPlayerUniqeID,
							Players.Players[PlayerIndex].CharacterCardType,
							Players.Players[PlayerIndex].JobCardType,
							CardList);
						ForceUpdate_AdvancePlayerTurn();
						return;
					}
				}
			}
		}
	}
}

void ABangGameMode::EndTurn(const uint32 UniqueID)
{
	if (UniqueID == CurrentTurnPlayerUniqeID)
	{
		if (CurrentPlayerTurnState == EPlayerTurnState::UseCard)
		{
			CurrentPlayerTurnState = EPlayerTurnState::LooseCard;
			AdvanceGameTurn();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[ABangGameMode::EndTurn] UniqueID: %d, CurrentPlayerTurn: %d"), UniqueID, CurrentPlayerTurnState);
}

void ABangGameMode::PlayerDead(const uint32 UniqueID,
                               const ECharacterType PlayerCharacter,
                               const EJobType JobType,
                               FPlayerCardCollection CardList)
{
	if (!CardManager) return;

	// 분기 처리시 return전에 모든작업 수행후 반환
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
	case EJobType::None:
		break;
	}

	// Message:: 사망처리 알림

	// 플레이어 제거
	for (uint16 i = 0; i < Players.Players.Num(); i++)
	{
		if (Players.Players[i].PlayerUniqueID == UniqueID)
		{
			Players.Players.RemoveAt(i);
			break;
		}
	}

	// 카드 제거
	for (auto [SymbolType, SymbolNumber] : CardList.PlayerCards)
	{
		FSingleCard FoundCard;
		CardManager->GetCardBySymbolAndNumber(SymbolType, SymbolNumber, EDeckType::HandedCard, FoundCard);
		// 카드 제거
		CardManager->ReorderUsedCards(FoundCard);
	}
}

// 심볼로 특정 카드 찾기 (Play Role)
void ABangGameMode::GetCardBySymbol(const FPlayerCardSymbol& Card)
{
	if (!CardManager) return;
	FSingleCard FoundCard;
	CardManager->GetCardBySymbolAndNumber(Card.SymbolType, Card.SymbolNumber, EDeckType::AvailCards, FoundCard);
	// 뽑은 후에 PS 전달
}

// 카드 뽑아서 PS에 전달 (플레이어에게 나눠줌)
void ABangGameMode::ForceUpdate_DrawCard(const uint32 UniqueID, const uint16 CardCount)
{
	if (!CardManager) return;
	
	FBangSinglePlayerState PlayerState;
	GetPlayerStatesByUniqueID(UniqueID, PlayerState);

	FCardCollection DrawCards;
	CardManager->HandCards(CardCount, DrawCards);

	PlayerState.State->PlayerInfo.GetPlayerInformation(UniqueID)->MyCards.AddCardCollectionToPlayerCards(DrawCards);
	PlayerState.State->ForceNetUpdate();
}

// 카드 뽑아서 선택 리스트에 전달 (플레이어에게 나눠줌)
void ABangGameMode::DrawCard(const uint16 CardCount)
{
	if (!CardManager) return;

	FCardCollection DrawCards;
	CardManager->HandCards(CardCount, DrawCards);

	FBangSinglePlayerState PlayerState;
	GetPlayerStatesByUniqueID(CurrentTurnPlayerUniqeID, PlayerState);

	FPlayerCollection PlayerCollection = PlayerState.State->PlayerInfo;
	FPlayerCardCollection PlayerCardCollection;

	for (auto [Card] : DrawCards.CardList)
	{
		FPlayerCardSymbol PlayerCard;
		PlayerCard.SymbolNumber = Card->SymbolNumber;
		PlayerCard.SymbolType = Card->SymbolType;
		PlayerCardCollection.PlayerCards.Add(PlayerCard);
	}

	PlayerState.State->PlayerInfo.SelectableCards = PlayerCardCollection;
	PlayerState.State->ForceNetUpdate();
}

// 카드 뽑아서 전체공개
void ABangGameMode::ShowCard(const uint16 CardCount)
{
	if (!CardManager) return;

	FCardCollection DrawCards;
	CardManager->ShowCards(CardCount, DrawCards);

	FBangSinglePlayerState PlayerState;
	GetPlayerStatesByUniqueID(CurrentTurnPlayerUniqeID, PlayerState);

	FPlayerCollection PlayerCollection = PlayerState.State->PlayerInfo;
	FPlayerCardCollection PlayerCardCollection;

	for (auto [Card] : DrawCards.CardList)
	{
		FPlayerCardSymbol PlayerCard;
		PlayerCard.SymbolNumber = Card->SymbolNumber;
		PlayerCard.SymbolType = Card->SymbolType;
		PlayerCardCollection.PlayerCards.Add(PlayerCard);
	}

	PlayerState.State->PlayerInfo.SelectableCards = PlayerCardCollection;
	PlayerState.State->ForceNetUpdate();
}

// 플레이어 HUD 노출
void ABangGameMode::UpdatePlayerHUD()
{
	for (const TObjectPtr<ABangPlayerController> BangPlayerController: BangPlayerControllers)
	{
		BangPlayerController->Client_DisplayBangUI();
	}
}

void ABangGameMode::RefundCards(const FPlayerCardCollection& RefundCard)
{
	if (!CardManager) return;

	for (auto [SymbolType, SymbolNumber] : RefundCard.PlayerCards)
	{
		FSingleCard SingleCard;
		CardManager->GetCardBySymbolAndNumber(SymbolType, SymbolNumber, EDeckType::HandedCard, SingleCard);
		CardManager->ReorderAvailCards(SingleCard);
	}
}

void ABangGameMode::CheckCardSymbol(const uint32& UniqueID, const uint16& CardCount)
{
	if (!CardManager) return;

	FCardCollection OutCards;
	CardManager->CheckCardSymbolFromAvailCards(CardCount, OutCards);

	FPlayerCardCollection PlayerCardCollection;

	for (FSingleCard CardList : OutCards.CardList)
	{
		FPlayerCardSymbol SinglePlayerCard;
		SinglePlayerCard.SymbolNumber = CardList.Card->SymbolNumber;
		SinglePlayerCard.SymbolType = CardList.Card->SymbolType;

		PlayerCardCollection.PlayerCards.Add(SinglePlayerCard);
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const TObjectPtr<ABangPlayerController> CastingController = Cast<ABangPlayerController>(It->Get());
		const TObjectPtr<ABangPlayerState> BangPlayerState = CastingController->GetPlayerState<ABangPlayerState>();

		BangPlayerState->Client_CheckCardSymbolReturn(UniqueID, PlayerCardCollection);
	}
}

void ABangGameMode::ForceUpdate_AdvancePlayerTurn()
{
	PlayerIndex++;
	PlayerIndex = PlayerIndex % Players.Players.Num();
	CurrentTurnPlayerUniqeID = Players.Players[PlayerIndex].PlayerUniqueID;

	FBangSinglePlayerState PlayerState;
	GetPlayerStatesByUniqueID(CurrentTurnPlayerUniqeID, PlayerState);

	PlayerState.State->PlayerInfo.GetPlayerInformation(CurrentTurnPlayerUniqeID)->bIsMyTurn = true;
	PlayerState.State->ForceNetUpdate();
	
	CurrentPlayerTurnState = EPlayerTurnState::DrawCard;
	
	AdvanceGameTurn();
}

// 원명 추가
void ABangGameMode::SetUserHP()
{
	if (BangPlayerControllers.Num() == 0) return;
	BangPlayerControllers[0]->SetInitializeHP(5);
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

    int32 PlayersNum = BangPlayerControllers.Num();
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
        SpawnParams.Owner = BangPlayerControllers[i];
        SpawnParams.Instigator = BangPlayerControllers[i]->GetPawn();
		
		PlayersTransfrom.Add(BangPlayerControllers[i], TPair<FVector, FRotator>(SpawnLocation, SpawnRotation));

        ABangCharacter* Player = GetWorld()->SpawnActor<ABangCharacter>(DefaultPawnClass, SpawnLocation, SpawnRotation, SpawnParams);
        if (Player)
        {
            BangPlayerControllers[i]->Possess(Player);
            UE_LOG(LogTemp, Error, TEXT("플레이어컨트롤러가 폰에 빙의했습니다."));
           
        	BangPlayerControllers[i]->Client_SetControllerRotation(SpawnRotation);
           //TODO : Suffle

            UE_LOG(LogTemp, Error, TEXT("Player Controller is %s"), *BangPlayerControllers[i]->GetName());

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

void ABangGameMode::OpenCamera(uint32 BangPlayerControllerID)
{
	ControllerIDAtCameraMode = BangPlayerControllerID;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABangPlayerController* PC = Cast<ABangPlayerController>(It->Get()))
		{
			//ControllerIDAtCameraMode = PC->GetUniqueID();
			//ControllerIDAtCameraMode = PC->PlayerUniqueID;
			bool bIsTarget = (PC->PlayerUniqueID == BangPlayerControllerID);
			
			//PC->Client_SetInputEnabled(bIsTarget);
			PC->Client_OpenCamera();
			PC->Client_SetOutline(PC->PlayerUniqueID, true, 252);
			if (bIsTarget)
			{
			//PC->Client_ToggleMappingContext(); //
				if (APawn* Pawn = PC->GetPawn())
				{
					if (ABangCharacter* Char = Cast<ABangCharacter>(Pawn))
					{
						Char->Multicast_SetOutline(true, 251);
					}
				}
			}
		}
	}
}

void ABangGameMode::CloseCamera()
{
	if (ControllerIDAtCameraMode == INDEX_NONE)
	{
		return;
	}
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABangPlayerController* PC = Cast<ABangPlayerController>(It->Get()))
		{
			bool bIsTarget = (PC->PlayerUniqueID == ControllerIDAtCameraMode);
			PC->Client_CloseCamera();
			if (bIsTarget)
			{
				PC->Client_ToggleMappingContext();
				if (APawn* Pawn = PC->GetPawn())
				{
					if (ABangCharacter* Char = Cast<ABangCharacter>(Pawn))
					{
						Char->Multicast_SetOutline(false, 0);
					}
				}
				PC->Client_ToggleMappingContext();
			}
			PC->Client_SetInputEnabled(true);
			PC->Client_SetOutline(PC->PlayerUniqueID, false, 0);
		}
	}
	ControllerIDAtCameraMode = INDEX_NONE;
}

void ABangGameMode::ShowTableCardsToAll(EShowTableCard ShowTableType)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABangPlayerController* PC = Cast<ABangPlayerController>(It->Get()))
		{	
			UE_LOG(LogTemp, Warning, TEXT("[ABangGameMode::DrawCardsAndNotifyClients] : 카드 전달: %s"), *PC->GetName());
			PC->Client_ShowDrawCard(ShowTableType);
		}
	}
}

/**
 * 게임 로그를 클라이언트들에게 브로드캐스트하기 위해 생성된 메서드입니다.
 *
 * @param Controller 게임 로그를 전송하려는 대상 컨트롤러. 이 컨트롤러는 ABangPlayerController로 캐스팅되어 사용됩니다.
 *
 */

void ABangGameMode::SendGameLog(const FString& GameLogMessage)
{
	if (ABangGameState* GS = GetGameState<ABangGameState>())
	{
		GS->BroadcastGameLogToClients(GameLogMessage);
	}
}

void ABangGameMode::ReSpawnPlayerAtTurn()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABangPlayerController* PC = Cast<ABangPlayerController>(It->Get()))
		{
			if (TPair<FVector, FRotator>* SpawnData = PlayersTransfrom.Find(PC))
			{
				if (APawn* Pawn = PC->GetPawn())
				{
					//TODO : 죽은사람은 빙의를 풀어서 소환을 못하게 한다.
					Pawn->SetActorLocationAndRotation(SpawnData->Key, SpawnData->Value);
					PC->Client_SetControllerRotation(SpawnData->Value);
				}
			}
		}
	}
	//여기에서 턴일 때 매핑처리 
	//턴인사람 제외하고는 못움직이게 IA_Move만 제거

}
void ABangGameMode::ReSpawnPlayerAtRestart()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABangPlayerController* PC = Cast<ABangPlayerController>(It->Get()))
		{
			if (TPair<FVector, FRotator>* SpawnData = PlayersTransfrom.Find(PC))
			{
				NewPossessCharacter(PC, SpawnData->Key, SpawnData->Value);
			}
		}
	}
}
void ABangGameMode::ReSpawnPlayerAtRestartBluePrint()
{
	ReSpawnPlayerAtTurn();
}
void ABangGameMode::NewPossessCharacter(AController* PlayerController, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (PlayerController == nullptr)
	{
		return;
	}

	APawn* BeforePawn = PlayerController->GetPawn();
	if (BeforePawn)
	{
		PlayerController->UnPossess();
		BeforePawn->Destroy();
	}

	TSubclassOf<APawn> PawnClass = DefaultPawnClass;
	if (PawnClass == nullptr)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = PlayerController;
	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (NewPawn)
	{
		PlayerController->Possess(NewPawn);
	}
	if (ABangPlayerController* BPC = Cast<ABangPlayerController>(PlayerController))
	{
		BPC->Client_SetControllerRotation(SpawnRotation);
	}
	
}

void ABangGameMode::AtPlayerDie(AController* DeadPlayerController, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (ABangPlayerController* PC = Cast<ABangPlayerController>(DeadPlayerController))
	{
		APawn* CurrentPawn = PC->GetPawn();
		if (CurrentPawn)
		{
			PC->UnPossess();
			CurrentPawn->Destroy();
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = PC;
			//여기에서 새로운 액터를 만들고 호출 
			// 위에있는 PlayerDead에서 이 함수 호출하면 될것 같습니다
			// Controller배열에서 빼주기 , 
			// 만일 매시를 바꾸게된다면 IsDead라는 변수가 하나 필요합니다 
			//APawn* NewPawn = GetWorld()->SpawnActor<APawn>(NewPawnClass, SpawnLocation, SpawnRotation, SpawnParams);

		/*	if (NewPawn)
			{
				PC->Possess(NewPawn);
			}*/
		}
	}
}
void ABangGameMode::DontStopTestBong()
{
	for (auto PC : BangPlayerControllers)
	{
		ABangPlayerController* tmp = Cast<ABangPlayerController>(PC);
		ABangPlayerState* tmpPS = Cast<ABangPlayerState>(tmp->PlayerState);
		auto TmpInfo = tmpPS->PlayerInfo.GetPlayerInformation(tmp->PlayerUniqueID);
		TmpInfo->bIsMyTurn = false;
	}
}
void ABangGameMode::MoveTestBong(int index)
{
	ABangPlayerController* tmp = Cast<ABangPlayerController>(BangPlayerControllers[index]);
	ABangPlayerState* tmpPS = Cast<ABangPlayerState>(tmp->PlayerState);
	auto TmpInfo = tmpPS->PlayerInfo.GetPlayerInformation(tmp->PlayerUniqueID);
	TmpInfo->bIsMyTurn = true;
}