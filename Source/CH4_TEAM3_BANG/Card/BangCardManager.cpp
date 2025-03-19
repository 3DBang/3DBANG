#include "BangCardManager.h"

#include "Card/BaseCard/BangCardBase.h"
#include "BangCardDataAsset.h"
#include "JobCard/BangJobCard.h"

UBangCardBase* UBangCardManager::GetItemByName(const FText Name) const
{
	if (CardData)
	{
		for (UBangCardBase* Card : CardData->Cards)
		{
			if (Card && Card->CardName.EqualTo(Name))
			{
				return Card;
			}
		}
	}
	return nullptr;
}

// 최초 게임 시작시에 실행
void UBangCardManager::GetAllCards()
{
	if (!CardData) return;

	AllCards.Empty();
	CardDeckByType.Empty();

	for (UBangCardBase* Card : CardData->Cards)
	{
		if (!Card) continue;

		AllCards.Add(Card);
		CardDeckByType.FindOrAdd(Card->CardType).Add(Card);
		switch (Card->CardType)
		{
			case ECardType::JobCard:
				{
					JobCards.Add(Card);
					break;
				}
			case ECardType::ActiveCard:
				{
					ActiveCards.Add(Card);
					break;
				}
			case ECardType::PassiveCard:
				{
					PassiveCards.Add(Card);
					break;
				}
			case ECardType::CharacterCard:
				{
					CharacterCards.Add(Card);
					break;
				}
		}
	}
}

// 인원에 맞는 직업카드 추출 로직
TArray<UBangCardBase*> UBangCardManager::GetJobByPlayer(int PlayerCount)
{
	if (PlayerCount < 4 || PlayerCount > 7) return {};
	
	TArray<UBangCardBase*> SelectedCards;

	int OfficerCount    = (PlayerCount >= 4) ? 1 : 0;
	int SubOfficerCount = (PlayerCount >= 5) ? (PlayerCount >= 7 ? 2 : 1) : 0;
	int OutlawCount     = (PlayerCount == 3) ? 1 : (PlayerCount == 4 || PlayerCount == 5) ? 2 : 3;
	int BetrayerCount   = 1;

	for (UBangCardBase* Card : JobCards)
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

	ShuffleArray(SelectedCards);
	
	return SelectedCards;
}

// 덱 섞기
void UBangCardManager::ShuffleDeck()
{
	if (CharacterCards.Num() == 0 && PassiveCards.Num() == 0 &&
		ActiveCards.Num() == 0 && JobCards.Num() == 0) return;

	ShuffleArray(CharacterCards);
	ShuffleArray(PassiveCards);
	ShuffleArray(ActiveCards);
	ShuffleArray(JobCards);
}

void UBangCardManager::ShuffleArray(TArray<UBangCardBase*>& Cards)
{
	if (Cards.Num() <= 1) return;

	const int32 LastIndex = Cards.Num() - 1;
	for (int32 i = LastIndex; i > 0; --i)
	{
		int32 RandomIndex = FMath::RandRange(0, i);
		Cards.Swap(i, RandomIndex);
	}
}