#include "Card/BangCardTableSpawner.h"
#include "Card/BangCardActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Card/BangCardManager.h"
#include "PlayerController/BangPlayerController.h"
#include "PlayerState/BangPlayerState.h"
#include "GameState/BangGameState.h"
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

	const FVector Center = GetActorLocation();
	const float Radius = 400.f;
	const float CardSpacing = 50.f;
	const float RowSpacing = 120.f;

	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* LocalPC = UGameplayStatics::GetPlayerController(World, 0);
	uint32 LocalUniqueID = -1;

	if (ABangPlayerController* LocalBangPC = Cast<ABangPlayerController>(LocalPC))
	{
		if (ABangPlayerState* LocalPS = LocalBangPC->GetPlayerState<ABangPlayerState>())
		{
			LocalUniqueID = LocalPS->PlayerUniqueID;
		}
	}

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABangPlayerController* PC = Cast<ABangPlayerController>(It->Get()))
		{
			if (ABangPlayerState* PS = Cast<ABangPlayerState>(PC->PlayerState))
			{
				uint32 UniqueID = PS->PlayerUniqueID;

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

				const float PlayerAngleDeg = (360.f / MaxPlayerCount) * PlayerIndex;
				const float PlayerAngleRad = FMath::DegreesToRadians(PlayerAngleDeg);
				FVector PlayerCenter = Center + FVector(FMath::Cos(PlayerAngleRad), FMath::Sin(PlayerAngleRad), 0.f) * Radius;

				FCardCollection HandCards;
				PS->GetCard(UniqueID, HandCards);

				const int32 CardsPerRow = 4;
				const int32 CardCount = HandCards.CardList.Num();
				const bool bIsMyCard = (UniqueID == LocalUniqueID);

				for (int32 CardIdx = 0; CardIdx < CardCount; ++CardIdx)
				{
					const int32 Row = CardIdx / CardsPerRow;
					const int32 Col = CardIdx % CardsPerRow;

					const float XOffset = (Col - (CardsPerRow - 1) / 2.0f) * CardSpacing;
					const float YOffset = (Row == 0) ? -20.f : -80.f;

					FVector ForwardVector = (Center - PlayerCenter).GetSafeNormal();
					FVector RightVector = FVector::CrossProduct(FVector::UpVector, ForwardVector);

					FVector CardPos = PlayerCenter + ForwardVector * YOffset + RightVector * XOffset;
					FRotator Rot = UKismetMathLibrary::FindLookAtRotation(CardPos, Center);

					if (ABangCardActor* Spawned = World->SpawnActor<ABangCardActor>(CardActorClass, CardPos + FVector(0, 0, 5), Rot))
					{
						Spawned->Multicast_SetCard(HandCards.CardList[CardIdx], bIsMyCard);
						SpawnedCardActors.Add(Spawned); 
					}
				}

				// 캐릭터 / 직업 카드
				{
					FVector ForwardVector = (Center - PlayerCenter).GetSafeNormal();
					FVector RightVector = FVector::CrossProduct(FVector::UpVector, ForwardVector);
					FVector LeftOffset = -RightVector * 150.f;
					FVector BasePos = PlayerCenter + LeftOffset + FVector(0, 0, 5.f);
					float EquipCardSpacing = 50.f;

					TArray<FSingleCard> EquipCards;

					FSingleCard CharacterCard;
					PS->GetCardByCharacter(PS->PlayerInfo.Players[PlayerIndex].CharacterCardType, CharacterCard);
					if (CharacterCard.Card != nullptr) EquipCards.Add(CharacterCard);

					FSingleCard JobCard;
					PS->GetCardByJobType(PS->PlayerInfo.Players[PlayerIndex].JobCardType, JobCard);
					if (JobCard.Card != nullptr) EquipCards.Add(JobCard);

					for (int32 i = 0; i < EquipCards.Num(); ++i)
					{
						FVector Offset = RightVector * ((i - (EquipCards.Num() - 1) / 2.0f) * EquipCardSpacing);
						FVector CardPos = BasePos + Offset;
						FRotator Rot = UKismetMathLibrary::FindLookAtRotation(CardPos, Center);

						if (ABangCardActor* Spawned = World->SpawnActor<ABangCardActor>(CardActorClass, CardPos, Rot))
						{
							Spawned->Multicast_SetCard(EquipCards[i], true);
							SpawnedCardActors.Add(Spawned); //  직업/캐릭터 카드도 저장
						}
					}
				}
			}
		}
	}
}

void ABangCardTableSpawner::SpawnUsedCards()
{
	if (!CardActorClass || !CardManager) return;

	if (CurrentUsedCardActor && CurrentUsedCardActor->IsValidLowLevel())
	{
		CurrentUsedCardActor->Destroy();
		CurrentUsedCardActor = nullptr;
	}

	const FVector Center = GetActorLocation();
	const FVector DeckPos = Center + FVector(-60, 0, 5);
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

void ABangCardTableSpawner::RemoveCardActor(const FSingleCard& CardToRemove)
{
	for (int32 i = SpawnedCardActors.Num() - 1; i >= 0; --i)
	{
		ABangCardActor* CardActor = SpawnedCardActors[i];
		if (CardActor && CardActor->CardData.Card &&
			CardActor->CardData.Card->SymbolType == CardToRemove.Card->SymbolType &&
			CardActor->CardData.Card->SymbolNumber == CardToRemove.Card->SymbolNumber)
		{
			CardActor->Destroy();
			SpawnedCardActors.RemoveAt(i);
			UE_LOG(LogTemp, Warning, TEXT("테이블에서 카드 제거됨: %s"), *CardToRemove.Card->GetName());
			break;
		}
	}
}

FVector ABangCardTableSpawner::GetEquipSlotPositionForPlayer(uint32 PlayerUniqueID, int32 EquipCardIndex) const
{
	const FVector Center = GetActorLocation();
	const float Radius = 400.f;
	const float EquipYOffset = 400.f; 
	const float EquipCardSpacing = 70.f; 

	UWorld* World = GetWorld();
	if (!World) return FVector::ZeroVector;

	int32 PlayerIndex = -1;

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABangPlayerController* PC = Cast<ABangPlayerController>(It->Get()))
		{
			if (ABangPlayerState* PS = Cast<ABangPlayerState>(PC->PlayerState))
			{
				for (int32 i = 0; i < PS->PlayerInfo.Players.Num(); ++i)
				{
					if (PS->PlayerInfo.Players[i].PlayerUniqueID == PlayerUniqueID)
					{
						PlayerIndex = i;
						break;
					}
				}
			}
		}
	}
	if (PlayerIndex == -1) return FVector::ZeroVector;

	const float PlayerAngleDeg = (360.f / MaxPlayerCount) * PlayerIndex;
	const float PlayerAngleRad = FMath::DegreesToRadians(PlayerAngleDeg);
	FVector PlayerCenter = Center + FVector(FMath::Cos(PlayerAngleRad), FMath::Sin(PlayerAngleRad), 0.f) * Radius;

	FVector ForwardVector = (Center - PlayerCenter).GetSafeNormal();
	FVector RightVector = FVector::CrossProduct(FVector::UpVector, ForwardVector);

	FVector EquipBasePos = PlayerCenter + ForwardVector * EquipYOffset + FVector(0, 0, 5.f);

	FVector Offset = RightVector * ((EquipCardIndex - 0.5f) * EquipCardSpacing);

	return EquipBasePos + Offset;
}



void ABangCardTableSpawner::RefreshEquippedCards()
{
	UWorld* World = GetWorld();
	if (!World) return;

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABangPlayerController* PC = Cast<ABangPlayerController>(It->Get()))
		{
			if (ABangPlayerState* PS = Cast<ABangPlayerState>(PC->PlayerState))
			{
				uint32 UniqueID = PS->PlayerUniqueID;

				FCardCollection EquippedCards;
				PS->GetEquippedCard(UniqueID, EquippedCards);

				for (int32 i = 0; i < EquippedCards.CardList.Num(); ++i)
				{
					const FSingleCard& EquipCard = EquippedCards.CardList[i];
					FVector Pos = GetEquipSlotPositionForPlayer(UniqueID, i);
					FRotator Rot = UKismetMathLibrary::FindLookAtRotation(Pos, GetActorLocation());

					for (ABangCardActor* CardActor : SpawnedCardActors)
					{
						if (CardActor && CardActor->CardData.Card &&
							CardActor->CardData.Card->SymbolType == EquipCard.Card->SymbolType &&
							CardActor->CardData.Card->SymbolNumber == EquipCard.Card->SymbolNumber)
						{
							CardActor->SetActorLocation(Pos);
							CardActor->SetActorRotation(Rot);
							break;
						}
					}
				}
			}
		}
	}
}
