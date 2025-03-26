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

	SpawnCardsOnTable(); 
}

void ABangCardTableSpawner::SpawnCardsOnTable()
{
	if (!CardActorClass || !CardManager) return;

	const FVector Center = GetActorLocation();
	const float Radius = 400.f;

	for (int32 i = 0; i < MaxPlayerCount; ++i)
	{
		float AngleDeg = (360.f / MaxPlayerCount) * i;
		float AngleRad = FMath::DegreesToRadians(AngleDeg);

		FVector Pos = Center + FVector(FMath::Cos(AngleRad), FMath::Sin(AngleRad), 0) * Radius;
		FRotator Rot = UKismetMathLibrary::FindLookAtRotation(Pos, Center);

		// 카드 1장 받기
		FCardCollection Cards;
		CardManager->HandCards(1, Cards);
		if (Cards.CardList.Num() > 0)
		{
			FSingleCard Card = Cards.CardList[0];

			if (ABangCardActor* Spawned = GetWorld()->SpawnActor<ABangCardActor>(CardActorClass, Pos, Rot))
			{
				Spawned->Multicast_SetCard(Card, true); // 모든 클라이언트에서 카드 앞면 보이게 처리
			}
		}
	}

	// 중앙 덱 (뒷면)
	for (int i = 0; i < 10; ++i)
	{
		FCardCollection Cards;
		CardManager->HandCards(1, Cards);
		if (Cards.CardList.Num() > 0)
		{
			FSingleCard Card = Cards.CardList[0];
			FVector DeckPos = Center + FVector(0, -100, i * 1.0f);

			if (ABangCardActor* DeckCard = GetWorld()->SpawnActor<ABangCardActor>(CardActorClass, DeckPos, FRotator::ZeroRotator))
			{
				DeckCard->Multicast_SetCard(Card, false); // 뒷면 카드
			}
		}
	}
}
