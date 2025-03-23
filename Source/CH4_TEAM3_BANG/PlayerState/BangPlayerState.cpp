#include "BangPlayerState.h"

#include "Card/BangCardManager.h"
#include "Card/BaseCard/BangCardBase.h"
#include "GameMode/BangGameMode.h"
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

void ABangPlayerState::OnRep_PlayerInfo()
{
	UE_LOG(LogTemp, Display, TEXT("OnRep_PlayerInfo"));
	const FString Message = FPlayerCollectionToString(PlayerInfo);
	GEngine->AddOnScreenDebugMessage(-1, 120.0f, FColor::Yellow, Message);
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
}

// 컨트롤러가 카드 조회 호출용
void ABangPlayerState::GetCard(const int32 InPlayerUniqueID, FCardCollection& OutCardCollection)
{
	if (InPlayerUniqueID == 0) return;

	FPlayerInformation* PlayerInformation = PlayerInfo.GetPlayerInformation(InPlayerUniqueID);
	for (auto [SymbolType, SymbolNumber] : PlayerInformation->MyCards.PlayerCards)
	{
		FSingleCard OutFoundCard;
		CardManager->GetCardBySymbolAndNumberFromDataAsset(SymbolType, SymbolNumber, OutFoundCard);
		OutCardCollection.CardList.Add(OutFoundCard);
	}
}

void ABangPlayerState::UseCard(const int32 FromUniqueID, const FSingleCard& SingleCard, const int32 ToUniqueID)
{
	// 카드 각종 분기 처리 카드를 사용할 수 없는 경우에는 PC에 응답 후 리턴처리
	if (!CardManager) return;

	// 거리 접근 가능한지 확인
	if (!PlayerInfo.IsDistanceAble(FromUniqueID, ToUniqueID))
	{
		// 거리 안된다고 PC에 알려줘야함
		return;
	}

	EActiveType OutActiveType;
	EPassiveType OutPassiveType;
	CardManager->GetCardTypeFromDataAsset(SingleCard.Card->SymbolType, SingleCard.Card->SymbolNumber, OutActiveType, OutPassiveType);

	switch (OutActiveType)
	{
	case EActiveType::None:
		break;
	case EActiveType::Bang:
		{
			FPlayerCardSymbol SingleSymbolCard;
			SingleSymbolCard.SymbolNumber = SingleCard.Card->SymbolNumber;
			SingleSymbolCard.SymbolType = SingleCard.Card->SymbolType;

			TObjectPtr<ABangPlayerState> OutPlayerState;
			FindTargetPlayerState(ToUniqueID, OutPlayerState);
			OutPlayerState->UseCardReturn(FromUniqueID, SingleSymbolCard, ToUniqueID, OutActiveType, EPassiveType::None);
			break;
		}
	case EActiveType::Missed:
		{
			
			break;
		}
	case EActiveType::Stagecoach:
		break;
	case EActiveType::WellsFargoBank:
		break;
	case EActiveType::Beer:
		break;
	case EActiveType::GatlingGun:
		break;
	case EActiveType::Robbery:
		break;
	case EActiveType::CatBalou:
		break;
	case EActiveType::Saloon:
		break;
	case EActiveType::Duel:
		break;
	case EActiveType::GeneralStore:
		break;
	case EActiveType::Indians:
		break;
	case EActiveType::Jail:
		break;
	case EActiveType::Dynamite:
		break;
	}

	switch (OutPassiveType)
	{
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
	
	if (ToUniqueID == 0) // 사용 대상이 없을때 (자기 자신한테 사용)
	{
		
	}

	RestoreCard(FromUniqueID, SingleCard);
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
		break;
	case EActiveType::Robbery:
		break;
	case EActiveType::CatBalou:
		break;
	case EActiveType::Saloon:
		break;
	case EActiveType::Duel:
		break;
	case EActiveType::GeneralStore:
		break;
	case EActiveType::Indians:
		break;
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

void ABangPlayerState::FindTargetPlayerState(const uint32 TargetUniqueID, TObjectPtr<ABangPlayerState>& OutPlayerState) const
{
	const TObjectPtr<UWorld> World = GetWorld();
	if (!World) return;

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (const APlayerController* PlayerController = It->Get())
		{
			if (ABangPlayerState* OtherPlayerState = PlayerController->GetPlayerState<ABangPlayerState>())
			{
				if (OtherPlayerState != this && OtherPlayerState->GetPlayerController()->GetUniqueID() == TargetUniqueID)
				{
					OutPlayerState = OtherPlayerState;
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

void ABangPlayerState::Server_DrawCard_Implementation(const uint32 FromUniqueID, const uint16 CardCount)
{
	const TObjectPtr<ABangGameMode> GameMode = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerState::Server_UseCard_Implementation] BeginPlay Controller GameMode is NULL!"));
		return;
	}
	
	GameMode->ForceUpdate_DrawCard(FromUniqueID, CardCount);
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

