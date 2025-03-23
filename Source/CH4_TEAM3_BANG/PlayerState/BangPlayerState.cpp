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
}

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

void ABangPlayerState::UseCard(const int32 FromUniqueID, FSingleCard SingleCard, const int32 ToUniqueID)
{
	// 카드 각종 분기 처리 카드를 사용할 수 없는 경우에는 PC에 응답 후 리턴처리

	if (ToUniqueID == 0) // 사용 대상이 없을때 (자기 자신한테 사용)
	{
		
		
		
		
	}

	RestoreCard(FromUniqueID, SingleCard);
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
									 Info.RangeToMe,
									 Info.RangeFromMe);

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

