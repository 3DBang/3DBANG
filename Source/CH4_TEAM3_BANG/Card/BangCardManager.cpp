#include "BangCardManager.h"

#include "Card/BaseCard/BangCardBase.h"
#include "BangCardDataAsset.h"
#include "JobCard/BangJobCard.h"

// 최초 게임 시작시에 실행
// GetAllCards()
// ShuffleDeck()
// ReorderCards()
// HandCards()

// 게임 시작시 호출
void UBangCardManager::PlayBeginByRole()
{
	GetAllCards();
	ShuffleDeck();
	ReorderCards();
}

// 카드를 정리한다 (사용된카드 -> 미사용카드)
void UBangCardManager::ReorderCards()
{
	if (CharacterCards.CardList.Num() == 0 && PassiveCards.CardList.Num() == 0 &&
		ActiveCards.CardList.Num() == 0 && JobCards.CardList.Num() == 0) return;

	if (UsedCards.CardList.Num() == 0 && HandedCards.CardList.Num() == 0 && AvailCards.CardList.Num() == 0) // 최초 실행 시
	{
		AvailCards.CardList.Append(PassiveCards.CardList);
		AvailCards.CardList.Append(ActiveCards.CardList);
	}
	else
	{
		ShuffleCards(UsedCards);
		AvailCards.CardList.Append(UsedCards.CardList);
		UsedCards.CardList.Empty();
	}
}

// 카드를 나눠준다.
void UBangCardManager::HandCards(const int CardCount, FCardCollection& OutCards_)
{
	if (AvailCards.CardList.Num() <= CardCount || AvailCards.CardList.Num() == 0)
	{
		ReorderCards();
	}

	for (int i = 0; i < CardCount; i++)
	{
		HandedCards.CardList.Add(AvailCards.CardList[0]);
		OutCards_.CardList.Add(AvailCards.CardList[0]);
		AvailCards.CardList.RemoveAt(0);
	}
}

// 모든 카드를 가져온다.
void UBangCardManager::GetAllCards()
{
	if (!CardData) return;

	AllCards.CardList.Empty();
	CharacterCards.CardList.Empty();
	PassiveCards.CardList.Empty();
	ActiveCards.CardList.Empty();
	JobCards.CardList.Empty();
	CardDeckByType.Empty();

	for (UBangCardBase* Card : CardData->Cards)
	{
		if (!Card) continue;

		AllCards.CardList.Add(Card);
		CardDeckByType.FindOrAdd(Card->CardType).CardList.Add(Card);
		switch (Card->CardType)
		{
			case ECardType::JobCard:
				{
					JobCards.CardList.Add(Card);
					break;
				}
			case ECardType::ActiveCard:
				{
					ActiveCards.CardList.Add(Card);
					break;
				}
			case ECardType::PassiveCard:
				{
					PassiveCards.CardList.Add(Card);
					break;
				}
			case ECardType::CharacterCard:
				{
					CharacterCards.CardList.Add(Card);
					break;
				}
		}
	}
}

// 인원에 맞는 직업카드 추출 로직
void UBangCardManager::GetJobByPlayer(const int PlayerCount, FCardCollection& SelectedCards_)
{
	if (PlayerCount < 4 || PlayerCount > 7) return;
	
	TArray<UBangCardBase*> SelectedCards;

	int OfficerCount    = (PlayerCount >= 4) ? 1 : 0;
	int SubOfficerCount = (PlayerCount >= 5) ? (PlayerCount >= 7 ? 2 : 1) : 0;
	int OutlawCount     = (PlayerCount == 3) ? 1 : (PlayerCount == 4 || PlayerCount == 5) ? 2 : 3;
	int BetrayerCount   = 1;

	for (UBangCardBase* Card : JobCards.CardList)
	{
		if (!Card) continue;

		if (Card->CardType == ECardType::JobCard)
		{
			if (const UBangJobCard* JobCard = Cast<UBangJobCard>(Card))
			{
				switch (JobCard->JobType)
				{
				case EJobType::Officer:
					{
						if (OfficerCount != 0)
						{
							SelectedCards.Add(Card);
							OfficerCount--;
						}
						break;
					}
				case EJobType::SubOfficer:
					{
						if (SubOfficerCount != 0)
						{
							SelectedCards.Add(Card);
							SubOfficerCount--;
						}
						break;
					}
				case EJobType::Outlaw:
					{
						if (OutlawCount != 0)
						{
							SelectedCards.Add(Card);
							OutlawCount--;
						}
						break;
					}
				case EJobType::Betrayer:
					{
						if (BetrayerCount != 0)
						{
							SelectedCards.Add(Card);
							BetrayerCount--;
						}
						break;
					}
				}
			}
		}
	}

	ShuffleCards(SelectedCards_);
}

// 덱 섞기
void UBangCardManager::ShuffleDeck()
{
	if (CharacterCards.CardList.Num() == 0 && PassiveCards.CardList.Num() == 0 &&
		ActiveCards.CardList.Num() == 0 && JobCards.CardList.Num() == 0) return;

	ShuffleCards(CharacterCards);
	ShuffleCards(PassiveCards);
	ShuffleCards(ActiveCards);
	ShuffleCards(JobCards);
}

void UBangCardManager::ShuffleCards(FCardCollection& Cards)
{
	if (Cards.CardList.Num() <= 1) return;

	const int32 LastIndex = Cards.CardList.Num() - 1;
	for (int32 i = LastIndex; i > 0; --i)
	{
		int32 RandomIndex = FMath::RandRange(0, i);
		Cards.CardList.Swap(i, RandomIndex);
	}
}