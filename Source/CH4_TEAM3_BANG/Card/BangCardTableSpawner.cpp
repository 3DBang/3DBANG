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
	const float CardAngleSpacing = 8.f; // ← 카드 간 각도 간격 

	for (int32 i = 0; i < MaxPlayerCount; ++i)
	{
		FVector PlayerCenter = Center + FVector(
			FMath::Cos(FMath::DegreesToRadians(360.f / MaxPlayerCount * i)),
			FMath::Sin(FMath::DegreesToRadians(360.f / MaxPlayerCount * i)),
			0.f
		) * Radius;

		FCardCollection Cards;
		CardManager->HandCards(4, Cards);
		const int32 CardCount = Cards.CardList.Num();

		for (int32 j = 0; j < CardCount; ++j)
		{
			const float AngleOffset = (-CardAngleSpacing * (CardCount - 1) / 2.0f) + j * CardAngleSpacing;
			const float AngleDeg = (360.f / MaxPlayerCount) * i + AngleOffset;
			const float AngleRad = FMath::DegreesToRadians(AngleDeg);

			FVector CardPos = Center + FVector(FMath::Cos(AngleRad), FMath::Sin(AngleRad), 0.f) * Radius;
			FRotator Rot = UKismetMathLibrary::FindLookAtRotation(CardPos, Center);

			if (ABangCardActor* Spawned = GetWorld()->SpawnActor<ABangCardActor>(CardActorClass, CardPos + FVector(0, 0, 5), Rot))
			{
				Spawned->Multicast_SetCard(Cards.CardList[j], true);
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
