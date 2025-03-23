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

