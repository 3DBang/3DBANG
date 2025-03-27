#include "Card/BangCardTableSpawner.h"
#include "Card/BangCardActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Card/BangCardManager.h"
#include "PlayerController/BangPlayerController.h"
#include "PlayerState/BangPlayerState.h"
ABangCardTableSpawner::ABangCardTableSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABangCardTableSpawner::BeginPlay()
{
	Super::BeginPlay();

}

void ABangCardTableSpawner::SpawnDeckCards()
{
	if (!CardActorClass) return;

	const FVector Center = GetActorLocation();
	const FVector BaseDeckPos = Center + FVector(-60, 0, 5);
	const int32 CardsToVisualize = 20;

	for (int i = 0; i < CardsToVisualize; ++i)
	{
		FCardCollection Cards;
		CardManager->HandCards(1, Cards);
		if (Cards.CardList.Num() > 0)
		{
			FSingleCard Card = Cards.CardList[0];
			FVector DeckPos = BaseDeckPos + FVector(0, 0, i * 0.5f);

			if (ABangCardActor* DeckCard = GetWorld()->SpawnActor<ABangCardActor>(CardActorClass, DeckPos, FRotator::ZeroRotator))
			{
				DeckCard->Multicast_SetCard(Card, false); // 뒷면
			}
		}
	}
}

void ABangCardTableSpawner::SpawnHandCards()
{
	if (!CardActorClass) return;

	// 카드 위치 조정 
	const FVector Center = GetActorLocation();
	const float Radius = 400.f;
	const float CardSpacing = 40.f;
	const float RowSpacing = 60.f;

	UWorld* World = GetWorld();
	if (!World) return;

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABangPlayerController* PC = Cast<ABangPlayerController>(It->Get()))
		{
			if (ABangPlayerState* PS = Cast<ABangPlayerState>(PC->PlayerState))
			{
				uint32 UniqueID = PS->PlayerUniqueID;

				//  PlayerIndex를 PlayerUniqueID 기준으로 찾아냄
				int32 PlayerIndex = -1;
				for (int32 i = 0; i < PS->PlayerInfo.Players.Num(); ++i)
				{
					if (PS->PlayerInfo.Players[i].PlayerUniqueID == UniqueID)
					{
						PlayerIndex = i;
						break;
					}
				}
				if (PlayerIndex == -1) continue;

				//  플레이어 위치 계산
				const float PlayerAngleDeg = (360.f / MaxPlayerCount) * PlayerIndex;
				const float PlayerAngleRad = FMath::DegreesToRadians(PlayerAngleDeg);
				FVector PlayerCenter = Center + FVector(FMath::Cos(PlayerAngleRad), FMath::Sin(PlayerAngleRad), 0.f) * Radius;

				FCardCollection HandCards;
				for (const FPlayerCardSymbol& Symbol : PS->PlayerInfo.Players[PlayerIndex].MyCards.PlayerCards)
				{
					if (!CardManager) continue;

					FSingleCard ConvertedCard;
					CardManager->GetCardBySymbolAndNumberFromDataAsset(Symbol.SymbolType, Symbol.SymbolNumber, ConvertedCard);

					if (ConvertedCard.Card != nullptr)
					{
						HandCards.CardList.Add(ConvertedCard);
					}
				}

				//  카드 위치 계산 & 스폰
				const int32 CardsPerRow = 6;
				const int32 CardCount = HandCards.CardList.Num();

				for (int32 CardIdx = 0; CardIdx < CardCount; ++CardIdx)
				{
					const int32 Row = CardIdx / CardsPerRow;
					const int32 Col = CardIdx % CardsPerRow;

					const float XOffset = (Col - (CardsPerRow - 1) / 2.0f) * CardSpacing;
					const float YOffset = (Row == 0) ? RowSpacing / 2 : -RowSpacing / 2;

					FVector CardPos = PlayerCenter;
					FVector ForwardVector = (Center - PlayerCenter).GetSafeNormal();
					FVector RightVector = FVector::CrossProduct(FVector::UpVector, ForwardVector);

					CardPos += ForwardVector * YOffset;
					CardPos += RightVector * XOffset;

					FRotator Rot = UKismetMathLibrary::FindLookAtRotation(CardPos, Center);

					if (ABangCardActor* Spawned = World->SpawnActor<ABangCardActor>(CardActorClass, CardPos + FVector(0, 0, 5), Rot))
					{
						Spawned->Multicast_SetCard(HandCards.CardList[CardIdx], true);
					}
				}
			}
		}
	}
}


void ABangCardTableSpawner::SpawnCardListOnTable(const FCardCollection& CardsToSpawn)
{
	if (!CardActorClass) return;

	const FVector Center = GetActorLocation() + FVector(0, 0, 100.f);
	const float CardSpacing = 100.f; // 카드 간격
	const float StartX = -((CardsToSpawn.CardList.Num() - 1) * CardSpacing) / 2.0f;

	for (int32 i = 0; i < CardsToSpawn.CardList.Num(); ++i)
	{
		const FSingleCard& Card = CardsToSpawn.CardList[i];
		FVector CardPos = Center + FVector(StartX + i * CardSpacing, 0.f, 0.f);
		FRotator Rot = FRotator::ZeroRotator;

		if (ABangCardActor* Spawned = GetWorld()->SpawnActor<ABangCardActor>(CardActorClass, CardPos, Rot))
		{
			Spawned->Multicast_SetCard(Card, true); // 앞면 
			UE_LOG(LogTemp, Log, TEXT("테이블에 카드 스폰됨: %s"), *Card.Card->CardName.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("카드 스폰 실패: %d번째"), i);
		}
	}
}

void ABangCardTableSpawner::SpawnUsedCards()
{

	if (!CardActorClass || !CardManager) return;

	// 이전 카드 삭제
	if (CurrentUsedCardActor && CurrentUsedCardActor->IsValidLowLevel())
	{
		CurrentUsedCardActor->Destroy();
		CurrentUsedCardActor = nullptr;
	}

	const FVector Center = GetActorLocation();
	const FVector DeckPos = Center + FVector(-60, 0, 5); // 덱 왼쪽 
	const FVector DeckToCenter = (Center - DeckPos).GetSafeNormal();
	const FVector RightVector = FVector::CrossProduct(DeckToCenter, FVector::UpVector);
	const FVector UsedCardPos = DeckPos + RightVector * 100.f + FVector(0, 0, 5);

	const FCardCollection& UsedCards = CardManager->GetUsedCards();
	if (UsedCards.CardList.Num() == 0) return;

	const FSingleCard& TopCard = UsedCards.CardList.Last();

	if (ABangCardActor* Spawned = GetWorld()->SpawnActor<ABangCardActor>(CardActorClass, UsedCardPos, FRotator::ZeroRotator))
	{
		Spawned->Multicast_SetCard(TopCard, true);
		CurrentUsedCardActor = Spawned;
	}
}
void ABangCardTableSpawner::SpawnEquippedCards()
{
	
}
