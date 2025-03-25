#include "BangPlayerState.h"

#include "Card/BangCardManager.h"
#include "Card/BaseCard/BangCardBase.h"
#include "Card/PassiveCard/BangPassiveCard.h"
#include "GameMode/BangGameMode.h"
#include "GameState/BangGameState.h"
#include "Instance/BangGameInstance.h"
#include "Net/UnrealNetwork.h"

ABangPlayerState::ABangPlayerState()
{
	bReplicates = true;
}

void ABangPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	if (const TObjectPtr<UBangGameInstance> BangGameInstance = Cast<UBangGameInstance>(GetGameInstance()))
	{
		FCardManagerInstance OutCardManager;
		BangGameInstance->GetCardManager(OutCardManager);
		CardManager = OutCardManager.CardManager;
		UE_LOG(LogTemp, Warning, TEXT("ABangPlayerState::CardManager Loaded"));
	}
}

void ABangPlayerState::Client_SetUniqueId_Implementation(const uint32& FromPlayerUniqueID)
{
	UE_LOG(LogTemp, Warning, TEXT("Client_SetUniqueId: [%d]"), FromPlayerUniqueID);
	PlayerUniqueID = FromPlayerUniqueID;
}

void ABangPlayerState::LoosePlayerHealth(const uint32& TargetUniqueID, int32 Amount)
{
	// Message::피 닳은거 알림
	PlayerInfo.GetPlayerInformation(TargetUniqueID)->CurrentHealth -= Amount;
	
	if (PlayerInfo.GetPlayerInformation(TargetUniqueID)->CurrentHealth <= 0)
	{
		// 사망처리
		Server_PlayerDead(TargetUniqueID);
	}
}

void ABangPlayerState::GainPlayerHealth(const uint32& TargetUniqueID, int32 Amount)
{
	PlayerInfo.GetPlayerInformation(TargetUniqueID)->CurrentHealth += Amount;
	
	if (PlayerInfo.GetPlayerInformation(TargetUniqueID)->MaxHealth < PlayerInfo.GetPlayerInformation(TargetUniqueID)->CurrentHealth)
	{
		PlayerInfo.GetPlayerInformation(TargetUniqueID)->CurrentHealth = PlayerInfo.GetPlayerInformation(TargetUniqueID)->MaxHealth;
	}
}

void ABangPlayerState::Server_SetPlayerInfo_Implementation(const FPlayerCollection& NewInfo)
{
	PlayerInfo = NewInfo;
	// OnRep_PlayerInfo() 호출
}

void ABangPlayerState::OnRep_PlayerInfo() // 클라만 반응
{
	UE_LOG(LogTemp, Display, TEXT("OnRep_PlayerInfo"));

	// 딜리게이트 뺴서 PC에서 GetCard 호출 UpdateCardList
	FOnPlayerInfoUpdated.Broadcast(PlayerInfo);
	
	if (const TObjectPtr<ABangPlayerController> BangPlayerController = Cast<ABangPlayerController>(GetPlayerController()))
	{
		BangPlayerController->PlayerUniqueID = PlayerUniqueID;
		UE_LOG(LogTemp, Display, TEXT("Update UniqueID {%d}{%d}"), PlayerUniqueID, BangPlayerController->PlayerUniqueID);
	}
	
	//const FString Message = FPlayerCollectionToString(PlayerInfo);
	//GEngine->AddOnScreenDebugMessage(-1, 120.0f, FColor::Yellow, Message);
	/*
	for (int32 i = 0; i < PlayerInfo.Players.Num(); ++i)
	{
		const FPlayerInformation& Info = PlayerInfo.Players[i];
		UE_LOG(LogTemp, Warning, TEXT("[OnRep] Player %d Info:"), i);
		UE_LOG(LogTemp, Warning, TEXT("  Name: %s, ID: %d"), *Info.PlayerName, Info.PlayerUniqueID);
		UE_LOG(LogTemp, Warning, TEXT("  HP: %d/%d"), Info.CurrentHealth, Info.MaxHealth);
		UE_LOG(LogTemp, Warning, TEXT("  RangeToMe: %d, RangeFromMe: %d"), Info.Range, Info.CharacterRange);
		UE_LOG(LogTemp, Warning, TEXT("  IsMyTurn: %s"), Info.bIsMyTurn ? TEXT("true") : TEXT("false"));
		UE_LOG(LogTemp, Warning, TEXT("  Job: %s, Character: %s"),
			*UEnum::GetValueAsString(Info.JobCardType),
			*UEnum::GetValueAsString(Info.CharacterCardType));
	}
	*/
}

// 컨트롤러가 카드타입 조회
void ABangPlayerState::GetCardType(const int32 InPlayerUniqueID, const FSingleCard& Card, EActiveType& OutActiveType, EPassiveType& OutPassiveType)
{
	if (InPlayerUniqueID == 0) return;
	
	CardManager->GetCardTypeFromDataAsset(Card.Card->SymbolType, Card.Card->SymbolNumber, OutActiveType, OutPassiveType);
}

// 컨트롤러가 카드 조회 응답용
void ABangPlayerState::Client_CheckCardSymbolReturn_Implementation(const uint32& FromUniqueID, const FPlayerCardCollection& PlayerCardCollection)
{
	for (FPlayerInformation Player : PlayerInfo.Players)
	{
		// Message::전역 메세지

		// 해당 플레이어
		if (Player.PlayerUniqueID == FromUniqueID)
		{
			for (auto [SymbolType, SymbolNumber] : PlayerCardCollection.PlayerCards)
			{
				if (SymbolType == ESymbolType::Heart)
				{
					// Message::피안단거 알림
					return;
				}
				LoosePlayerHealth(FromUniqueID, 1);
			}
		}
	}
}

// 컨트롤러가 카드 조회 호출용
void ABangPlayerState::GetCard(const int32 InPlayerUniqueID, FCardCollection& OutCardCollection)
{
	if (InPlayerUniqueID == 0 || !CardManager) return;

	if (FPlayerInformation* PlayerInformation = PlayerInfo.GetPlayerInformation(InPlayerUniqueID))
	{
		for (auto [SymbolType, SymbolNumber] : PlayerInformation->MyCards.PlayerCards)
		{
			FSingleCard OutFoundCard;
			CardManager->GetCardBySymbolAndNumberFromDataAsset(SymbolType, SymbolNumber, OutFoundCard);
			UE_LOG(LogTemp, Error, TEXT("[ABangPlayerState::GetCard] Player ID: %d %s"), InPlayerUniqueID, *OutFoundCard.Card->CardName.ToString());
			OutCardCollection.CardList.Add(OutFoundCard);
		}
	}
}

void ABangPlayerState::GetCard(const int32 InPlayerUniqueID, FCardCollection& OutCardCollection, FPlayerCollection _PlayerInfo)
{
	if (InPlayerUniqueID == 0 || !CardManager) return;
	
	const FString UniqueNetId = GetUniqueId().GetUniqueNetId()->ToString();
	UE_LOG(LogTemp, Warning, TEXT("[ABangPlayerState::GetCard] Player UniqueNetId: %s"), *UniqueNetId);
	
	FPlayerInformation* PlayerInformation = _PlayerInfo.GetPlayerInformation(InPlayerUniqueID);
	UE_LOG(LogTemp, Error, TEXT("[ABangPlayerState::GetCard] PlayerInformation: %d"), PlayerInformation->PlayerUniqueID);
	
	for (auto [SymbolType, SymbolNumber] : PlayerInformation->MyCards.PlayerCards)
	{
		FSingleCard OutFoundCard;
		CardManager->GetCardBySymbolAndNumberFromDataAsset(SymbolType, SymbolNumber, OutFoundCard);
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerState::GetCard] Player ID: %d %s"), InPlayerUniqueID, *OutFoundCard.Card->CardName.ToString());
		OutCardCollection.CardList.Add(OutFoundCard);
	}
}

void ABangPlayerState::UseCard(const int32 FromUniqueID, const FSingleCard& SingleCard, const int32 ToUniqueID)
{
	// 카드 각종 분기 처리 카드를 사용할 수 없는 경우에는 PC에 응답 후 리턴처리
	if (!CardManager) return;

	EActiveType OutActiveType;
	EPassiveType OutPassiveType;
	CardManager->GetCardTypeFromDataAsset(SingleCard.Card->SymbolType, SingleCard.Card->SymbolNumber, OutActiveType, OutPassiveType);

	switch (OutActiveType)
	{
	case EActiveType::None:
		break;
	case EActiveType::Bang:
		{
			if (ToUniqueID == 0) return;
			// 거리 접근 가능한지 확인
			if (!PlayerInfo.IsBangDistanceAble(FromUniqueID, ToUniqueID))
			{
				// 거리 안된다고 PC에 알려줘야함
				return;
			}
			
			FPlayerCardSymbol SingleSymbolCard;
			SingleSymbolCard.SymbolNumber = SingleCard.Card->SymbolNumber;
			SingleSymbolCard.SymbolType = SingleCard.Card->SymbolType;

			FBangSinglePlayerState OutPlayerState;
			FindTargetPlayerState(ToUniqueID, OutPlayerState);
			OutPlayerState.State->UseCardReturn(FromUniqueID, SingleSymbolCard, ToUniqueID, OutActiveType, EPassiveType::None);
			break;
		}
	case EActiveType::Missed:
		{
			// PC 에서 못쓰게 막음
			break;
		}
	case EActiveType::Stagecoach:
		{
			Server_DrawCard(FromUniqueID, 2, true);
			break;
		}
	case EActiveType::WellsFargoBank:
		{
			Server_DrawCard(FromUniqueID, 3, true);
			break;
		}
	case EActiveType::Beer:
		{
			PlayerInfo.GetPlayerInformation(FromUniqueID)->CurrentHealth++;
			break;
		}
	case EActiveType::GatlingGun:
		{
			// 모든 플레이어 공격 빗나감으로 막을수 있음 술통 사용가능 거리 x
			// 1. 술통 회피
			// 2. 빗나감 사용여부 응답
			FPlayerCardSymbol SingleSymbolCard;
			SingleSymbolCard.SymbolNumber = SingleCard.Card->SymbolNumber;
			SingleSymbolCard.SymbolType = SingleCard.Card->SymbolType;
			
			for (FPlayerInformation Player : PlayerInfo.Players)
			{
				if (FromUniqueID == Player.PlayerUniqueID) continue;

				FBangSinglePlayerState OutPlayerState;
				FindTargetPlayerState(ToUniqueID, OutPlayerState);
				OutPlayerState.State->UseCardReturn(FromUniqueID, SingleSymbolCard, ToUniqueID, OutActiveType, EPassiveType::None);
			}
			
			break;
		}
	case EActiveType::Robbery:
		{
			if (ToUniqueID == 0) return;
			// 거리 접근 가능한지 확인
			if (!PlayerInfo.IsDistanceAble(FromUniqueID, ToUniqueID))
			{
				// 거리 안된다고 PC에 알려줘야함
				return;
			}

			// PC에서 뺏을 카드 선택 ToUniqueID
			
			break;
		}
	case EActiveType::CatBalou:
		{
			if (ToUniqueID == 0) return;
			// 거리 x

			// PC 에 지목할카드
			break;
		}
	case EActiveType::Saloon:
		{
			// 모든 플레이어 체력 회복
			for (FPlayerInformation Player : PlayerInfo.Players)
			{
				GainPlayerHealth(Player.PlayerUniqueID, 1);
			}
			break;
		}
	case EActiveType::Duel:
		{
			if (ToUniqueID == 0) return;
			// PC에 전달
			FPlayerCardSymbol SingleSymbolCard;
			SingleSymbolCard.SymbolNumber = SingleCard.Card->SymbolNumber;
			SingleSymbolCard.SymbolType = SingleCard.Card->SymbolType;
			
			UseCardReturn(FromUniqueID, SingleSymbolCard, ToUniqueID, OutActiveType, EPassiveType::None);
			break;
		}
	case EActiveType::GeneralStore:
		{
			FPlayerCardSymbol SingleSymbolCard;
			SingleSymbolCard.SymbolNumber = SingleCard.Card->SymbolNumber;
			SingleSymbolCard.SymbolType = SingleCard.Card->SymbolType;
			
			Server_DrawCard(FromUniqueID, PlayerInfo.Players.Num(), false);
			UseCardReturn(FromUniqueID, SingleSymbolCard, ToUniqueID, OutActiveType, EPassiveType::None);
			break;
		}
	case EActiveType::Indians:
		{
			FPlayerCardSymbol SingleSymbolCard;
			SingleSymbolCard.SymbolNumber = SingleCard.Card->SymbolNumber;
			SingleSymbolCard.SymbolType = SingleCard.Card->SymbolType;

			UseCardReturn(FromUniqueID, SingleSymbolCard, ToUniqueID, OutActiveType, EPassiveType::None);
			break;
		}
	case EActiveType::Jail:
		{
			if (ToUniqueID == 0) return;
			if (PlayerInfo.GetPlayerInformation(ToUniqueID)->JobCardType == EJobType::Officer)
			{
				// 보완관한테는 감옥 못 씀
				return;
			}
			FPlayerCardSymbol SingleSymbolCard;
			SingleSymbolCard.SymbolNumber = SingleCard.Card->SymbolNumber;
			SingleSymbolCard.SymbolType = SingleCard.Card->SymbolType;

			UseCardReturn(FromUniqueID, SingleSymbolCard, ToUniqueID, OutActiveType, EPassiveType::None);
			break;
		}
	case EActiveType::Dynamite:
		{
			if (ToUniqueID == 0) return;

			break;
		}
	}

	if (OutPassiveType != EPassiveType::None)
	{
		if (CheckIsCardAble(FromUniqueID, SingleCard))
		{
			FPlayerCardSymbol SingleSymbolCard;
			SingleSymbolCard.SymbolNumber = SingleCard.Card->SymbolNumber;
			SingleSymbolCard.SymbolType = SingleCard.Card->SymbolType;
			PlayerInfo.GetPlayerInformation(FromUniqueID)->EquippedCards.PlayerCards.Add(SingleSymbolCard);
		}
		else
		{
			// PC에 장착 불가라고 호출
			return;
		}
		
		switch (OutPassiveType)
		{
		case EPassiveType::None:
			break;
		case EPassiveType::Barrel:
			break;
		case EPassiveType::Scope:
			{
				PlayerInfo.GetPlayerInformation(FromUniqueID)->Range++;
				break;
			}
		case EPassiveType::Mustang:
			{
				PlayerInfo.GetPlayerInformation(FromUniqueID)->CharacterRange++;
				break;
			}
		case EPassiveType::Schofield:
			{
				PlayerInfo.GetPlayerInformation(FromUniqueID)->GunRange = 2;
				break;
			}
		case EPassiveType::Volcanic:
			{
				PlayerInfo.GetPlayerInformation(FromUniqueID)->GunRange = 1;
				break;
			}
		case EPassiveType::Remington:
			{
				PlayerInfo.GetPlayerInformation(FromUniqueID)->GunRange = 3;
				break;
			}
		case EPassiveType::Carbine:
			{
				PlayerInfo.GetPlayerInformation(FromUniqueID)->GunRange = 4;
				break;
			}
		case EPassiveType::Winchester:
			{
				PlayerInfo.GetPlayerInformation(FromUniqueID)->GunRange = 5;
				break;
			}
		}
	}

	RestoreCard(FromUniqueID, SingleCard);
}

// 패시브 카드 중복 장착 방지용
bool ABangPlayerState::CheckIsCardAble(const int32 FromUniqueID, const FSingleCard& SingleCard)
{
	if (!CardManager) return false;
	
	for (auto [SymbolType, SymbolNumber] : PlayerInfo.GetPlayerInformation(FromUniqueID)->EquippedCards.PlayerCards)
	{
		FSingleCard OutFoundCard;
		CardManager->GetCardBySymbolAndNumberFromDataAsset(SymbolType, SymbolNumber, OutFoundCard);

		if (SingleCard == OutFoundCard)
		{
			return false;
		}
	}

	return true;
}

void ABangPlayerState::Server_CheckCardSymbol_Implementation(const uint32& FromUniqueID, const uint16& CardCount)
{
	const TObjectPtr<ABangGameMode> GameMode = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerState::Server_UseCard_Implementation] BeginPlay Controller GameMode is NULL!"));
		return;
	}

	GameMode->CheckCardSymbol(FromUniqueID, CardCount);
}

// 카드 사용에 대한 응답 (상호작용)
void ABangPlayerState::UseCardReturn(const int32& FromUniqueID, const FPlayerCardSymbol& SingleCard, const int32& ToUniqueID, const EActiveType& ActiveType, const EPassiveType& PassiveType)
{
	if (!CardManager) return;
	
	switch (ActiveType)
	{
	case EActiveType::None:
		break;
	case EActiveType::Bang:
		{
			FPlayerCollection PlayerCollection;
			if (PlayerCollection.GetPlayerInformation(FromUniqueID)->CharacterCardType == ECharacterType::SlabTheKiller)
			{
				// 빗나감 두개 써야 막아지도록 PC에서 설정	
			}
			else if (PlayerCollection.GetPlayerInformation(FromUniqueID)->CharacterCardType == ECharacterType::Jourdonnais)
			{
				// 카드 펼치기 해야함
				Server_CheckCardSymbol(ToUniqueID, 1);
				return;
			}
		}
	case EActiveType::Missed:
		break;
	case EActiveType::Stagecoach:
		break;
	case EActiveType::WellsFargoBank:
		break;
	case EActiveType::Beer:
		break;
	case EActiveType::GatlingGun:
		{
			for (FPlayerCardSymbol PlayerCard : PlayerInfo.GetPlayerInformation(ToUniqueID)->EquippedCards.PlayerCards)
			{
				FSingleCard OutFoundCard;
				CardManager->GetCardBySymbolAndNumberFromDataAsset(PlayerCard.SymbolType, PlayerCard.SymbolNumber, OutFoundCard);

				if (TObjectPtr<UBangPassiveCard> PassiveCard = Cast<UBangPassiveCard>(OutFoundCard.Card))
				{
					if (PassiveCard->PassiveType == EPassiveType::Barrel)
					{
						
					}
				}
			}
			
			//PlayerInfo.GetPlayerInformation(ToUniqueID)->MyCards.PlayerCards.Contains();
			break;
		}
	case EActiveType::Robbery:
		break;
	case EActiveType::CatBalou:
		break;
	case EActiveType::Saloon:
		break;
	case EActiveType::Duel:
		{
			// PC에 뱅 사용여부 전달
			break;
		}
	case EActiveType::GeneralStore:
		{
			// PC에 카드 선택 요구 SelectableCards에서 하나 선택하고 MyCards로 갱신한뒤 SelectableCards 다 삭제해야함
			break;
		}
	case EActiveType::Indians:
		{
			// PC에 뱅 내라고 요청
			break;
		}
	case EActiveType::Jail:
		break;
	case EActiveType::Dynamite:
		break;
	}

	switch (PassiveType) {
	case EPassiveType::None:
		break;
	case EPassiveType::Barrel:
		break;
	case EPassiveType::Scope:
		break;
	case EPassiveType::Mustang:
		break;
	case EPassiveType::Schofield:
		break;
	case EPassiveType::Volcanic:
		break;
	case EPassiveType::Remington:
		break;
	case EPassiveType::Carbine:
		break;
	case EPassiveType::Winchester:
		break;
	}
}

void ABangPlayerState::FindTargetPlayerState(const uint32 TargetUniqueID, FBangSinglePlayerState& OutPlayerState) const
{
	const TObjectPtr<UWorld> World = GetWorld();
	if (!World) return;

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (const APlayerController* PlayerController = It->Get())
		{
			if (ABangPlayerState* OtherPlayerState = PlayerController->GetPlayerState<ABangPlayerState>())
			{
				if (OtherPlayerState != this && OtherPlayerState->PlayerUniqueID == TargetUniqueID)
				{
					OutPlayerState.State = OtherPlayerState;
					return;
				}
			}
		}
	}
}

void ABangPlayerState::Server_UseCard_Implementation(const int32 FromUniqueID, const ESymbolType SymbolType, const int32 SymbolNumber, const EDeckType DeckType)
{
	const TObjectPtr<ABangGameMode> GameMode = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerState::Server_UseCard_Implementation] BeginPlay Controller GameMode is NULL!"));
		return;
	}

	GameMode->ForceUpdate_LooseCardFromHanded(FromUniqueID, SymbolType, SymbolNumber, DeckType);
}

void ABangPlayerState::UseCardToAll(const int32 FromUniqueID, FSingleCard SingleCard)
{
	
}

void ABangPlayerState::RestoreCard(const int32 FromUniqueID, FSingleCard SingleCard)
{
	// 서버에서 카드 지움
	Server_UseCard(FromUniqueID, SingleCard.Card->SymbolType, SingleCard.Card->SymbolNumber, EDeckType::HandedCard);
}

void ABangPlayerState::StartTurn(const int32 InPlayerUniqueID, FCardCollection& DrawCards)
{
	/*
	const TObjectPtr<UWorld> World = GetWorld();
	if (!World || InPlayerUniqueID == 0)
	{
		return;
	}
	
	// 플레이어 턴으로 변경 및 인포에 카드 추가
	FPlayerInformation* PlayerInformation = PlayerInfo.GetPlayerInformation(InPlayerUniqueID);
	PlayerInformation->bIsMyTurn = true;
	PlayerInformation->MyCards.AddCardCollectionToPlayerCards(DrawCards);
	ForceNetUpdate();
	
	// 플레이어들을 순회
	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		//모든 플레이어의 컨트롤러를 순환
		ABangPlayerController* PlayerController = Cast<ABangPlayerController>(It->Get());

		//플레이어 컨트롤러의 스테이트
		ABangPlayerState* OtherPlayerState = PlayerController->GetPlayerState<ABangPlayerState>();

		//게임 스테이트
		ABangGameState* GameState = Cast<ABangGameState>(World->GetGameState());
		if (!PlayerController || !OtherPlayerState || !GameState)
		{
			return;
		}

		//전체에게 메세지 뿌리기
		FString ChatMessage = FString::Printf(TEXT("플레이어 %s의 차례!"), *PlayerInformation->PlayerName);  
		FString FromNickname = TEXT("Server"); 
		FString ReciverNickname = TEXT("All"); 
		GameState->ReceiveMessage(ChatMessage, FromNickname, ReciverNickname);
		
		// 현재 플레이어 턴이면
		if (PlayerController->PlayerUniqueID == InPlayerUniqueID)
		{
			PlayerController->Client_OnTurnStart(DrawCards);
		}
	}
	*/
}

// 턴 종료 모든 처리 끝나면 호출
void ABangPlayerState::EndTurn(const int32 InPlayerUniqueID)
{
	if (InPlayerUniqueID == 0) return;
	PlayerInfo.GetPlayerInformation(InPlayerUniqueID)->bIsMyTurn = false;
	Server_EndTurn(InPlayerUniqueID);
}

void ABangPlayerState::Server_PlayerDead_Implementation(const int32 FromUniqueID)
{
	const TObjectPtr<ABangGameMode> GameMode = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerState::Server_UseCard_Implementation] BeginPlay Controller GameMode is NULL!"));
		return;
	}

	const ECharacterType PlayerCharacter = PlayerInfo.GetPlayerInformation(FromUniqueID)->CharacterCardType;
	const EJobType JobType = PlayerInfo.GetPlayerInformation(FromUniqueID)->JobCardType;
	FPlayerCardCollection CardList;
	PlayerInfo.GetPlayerInformation(FromUniqueID)->GetAllCardList(CardList);
	
	GameMode->PlayerDead(FromUniqueID, PlayerCharacter, JobType, CardList);
}

void ABangPlayerState::Server_EndTurn_Implementation(const int32 InPlayerUniqueID)
{
	const TObjectPtr<ABangGameMode> GameMode = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerState::Server_UseCard_Implementation] BeginPlay Controller GameMode is NULL!"));
		return;
	}

	GameMode->EndTurn(InPlayerUniqueID);
}

void ABangPlayerState::Server_DrawCard_Implementation(const uint32 FromUniqueID, const uint16 CardCount, const bool bIsForce)
{
	const TObjectPtr<ABangGameMode> GameMode = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerState::Server_UseCard_Implementation] BeginPlay Controller GameMode is NULL!"));
		return;
	}

	if (bIsForce)
	{
		GameMode->ForceUpdate_DrawCard(FromUniqueID, CardCount);
	}
	else
	{
		GameMode->DrawCard(CardCount);
	}
}

void ABangPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// PlayerStat을 모든 클라에 복제하겠다는 뜻
	DOREPLIFETIME(ABangPlayerState, PlayerInfo);
}

// 카드매니저에서 카드정보 가져오기
FCardCollection ABangPlayerState::GetCardListFromCardManager(const FPlayerInformation& Info) const
{
	FCardCollection CardCollection;
	
	for (auto [SymbolType, SymbolNumber] : Info.MyCards.PlayerCards)
	{
		FSingleCard OutFoundCard;
		CardManager->GetCardBySymbolAndNumberFromDataAsset(SymbolType, SymbolNumber,OutFoundCard);
		CardCollection.CardList.Add(OutFoundCard);
	}
	
	return CardCollection;
}

// 로그용
FString ABangPlayerState::FPlayerInformationToString(const FPlayerInformation& Info)
{
	FString String = FString::Printf(TEXT("ID: %u, Name: %s, HP: %d/%d, RangeToMe: %d, RangeFromMe: %d"),
									 Info.PlayerUniqueID,
									 *Info.PlayerName,
									 Info.CurrentHealth,
									 Info.MaxHealth,
									 Info.Range,
									 Info.CharacterRange);
	
	FCardCollection CardCollection = GetCardListFromCardManager(Info);

	for (FSingleCard CardList : CardCollection.CardList)
	{
		String += FString::Printf(TEXT(" Card: %s"), *CardList.Card->CardName.ToString());
	}
	
	return String;
}

FString ABangPlayerState::FPlayerCollectionToString(const FPlayerCollection& Collection)
{
	FString Output = TEXT("Player List:\n");
	for (const auto& Info : Collection.Players)
	{
		Output += FPlayerInformationToString(Info) + TEXT("\n");
	}
	return Output;
}

