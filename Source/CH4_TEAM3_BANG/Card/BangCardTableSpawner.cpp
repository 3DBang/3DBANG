#include "Card/BangCardTableSpawner.h"
#include "Card/BangCardActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Card/BangCardManager.h"

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
	if (!CardActorClass || !CardManager) return;

	const FVector Center = GetActorLocation();
	const float Radius = 400.f;
	const float CardSpacing = 40.f;   // 카드 간격 (좌우)
	const float RowSpacing = 60.f;    // 두 줄 사이 간격 (위아래)

	for (int32 PlayerIndex = 0; PlayerIndex < MaxPlayerCount; ++PlayerIndex)
	{
		// 플레이어의 중심 위치 구하기
		const float PlayerAngleDeg = (360.f / MaxPlayerCount) * PlayerIndex;
		const float PlayerAngleRad = FMath::DegreesToRadians(PlayerAngleDeg);
		FVector PlayerCenter = Center + FVector(FMath::Cos(PlayerAngleRad), FMath::Sin(PlayerAngleRad), 0.f) * Radius;

		FCardCollection Drawn;
		CardManager->HandCards(8, Drawn);  // 플레이어당 카드 8장

		const int32 CardsPerRow = 4;  // 한 줄당 최대 카드 수 (여기서는 최대 5장)
		const int32 CardCount = Drawn.CardList.Num();

		// 카드 배치를 위아래 두 줄로 나눔
		for (int32 CardIdx = 0; CardIdx < CardCount; ++CardIdx)
		{
			const int32 Row = CardIdx / CardsPerRow;          // 위(0) 또는 아래(1) 줄
			const int32 Col = CardIdx % CardsPerRow;          // 열 (좌우)

			// 카드의 X축 위치 (가로 방향으로 나열)
			const float XOffset = (Col - (CardsPerRow - 1) / 2.0f) * CardSpacing;

			// 위아래로 배치할 Z축 위치 (위줄 아래줄)
			const float YOffset = (Row == 0) ? RowSpacing / 2 : -RowSpacing / 2;

			// 최종 카드 위치 (플레이어 기준)
			FVector CardPos = PlayerCenter;

			// 플레이어의 방향을 기준으로 위치 조정
			FVector ForwardVector = (Center - PlayerCenter).GetSafeNormal();
			FVector RightVector = FVector::CrossProduct(FVector::UpVector, ForwardVector);

			CardPos += ForwardVector * YOffset;  // 앞뒤 (위아래줄)
			CardPos += RightVector * XOffset;    // 좌우 배치

			// 카드의 방향은 항상 테이블 중앙을 향하게 설정
			FRotator Rot = UKismetMathLibrary::FindLookAtRotation(CardPos, Center);

			if (ABangCardActor* Spawned = GetWorld()->SpawnActor<ABangCardActor>(CardActorClass, CardPos + FVector(0, 0, 5), Rot))
			{
				Spawned->Multicast_SetCard(Drawn.CardList[CardIdx], true);
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