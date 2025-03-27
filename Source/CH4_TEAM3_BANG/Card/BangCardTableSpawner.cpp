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

	SpawnDeckCards();
	SpawnHandCards();
}

void ABangCardTableSpawner::SpawnDeckCards()
{
	if (!CardActorClass) return;

	const FVector Center = GetActorLocation();
	const FVector BaseDeckPos = Center + FVector(0, -100, 0);
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

	const FVector Center = GetActorLocation();
	const float Radius = 400.f;

	for (int32 i = 0; i < MaxPlayerCount; ++i)
	{
		float AngleDeg = (360.f / MaxPlayerCount) * i;
		float AngleRad = FMath::DegreesToRadians(AngleDeg);

		FVector PlayerCenter = Center + FVector(FMath::Cos(AngleRad), FMath::Sin(AngleRad), 0) * Radius;
		FRotator Rot = UKismetMathLibrary::FindLookAtRotation(PlayerCenter, Center);

		FCardCollection Cards;
		CardManager->HandCards(4, Cards);

		UE_LOG(LogTemp, Warning, TEXT(" Player %d에게 카드 %d장 지급"), i + 1, Cards.CardList.Num());

		const float CardSpacing = 75;
		const int32 CardCount = Cards.CardList.Num();
		const float StartOffset = -((CardCount - 1) * CardSpacing) / 2.0f;

		for (int32 j = 0; j < CardCount; ++j)
		{
			const float YOffset = StartOffset + j * CardSpacing;
			const FVector CardPos = PlayerCenter + FVector(0.f, YOffset, 0.f);

			if (ABangCardActor* Spawned = GetWorld()->SpawnActor<ABangCardActor>(CardActorClass, CardPos, Rot))
			{
				UE_LOG(LogTemp, Log, TEXT("    → 카드 %d 스폰 완료!"), j + 1);
				Spawned->Multicast_SetCard(Cards.CardList[j], true);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("    → 카드 %d 스폰 실패!"), j + 1);
			}
		}
	}
}