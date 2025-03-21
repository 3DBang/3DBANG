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

// 카드 심볼과 번호로 카드 찾기
void UBangCardManager::GetCardBySymbolAndNumber(const ESymbolType SymbolType, const int32 SymbolNumber, const bool IsFromHanded, FSingleCard& FoundCard_)
{
	if (HandedCards.CardList.Num() == 0 && UsedCards.CardList.Num() == 0) return;
	
	if (IsFromHanded)
	{
		for (const FSingleCard HandedCard : HandedCards.CardList)
		{
			if (HandedCard.Card->SymbolType == SymbolType && HandedCard.Card->SymbolNumber == SymbolNumber)
			{
				FoundCard_ = HandedCard;
				break;
			}
		}
	}
	else
	{
		for (const FSingleCard UsedCard : UsedCards.CardList)
		{
			if (UsedCard.Card->SymbolType == SymbolType && UsedCard.Card->SymbolNumber == SymbolNumber)
			{
				FoundCard_ = UsedCard;
				break;
			}
		}
	}
}

// 건내준 카드를 다시 사용된 카드 덱에 넣는다
void UBangCardManager::ReorderUsedCards(const FSingleCard HandedCard)
{
	if (HandedCards.CardList.Num() == 0) return;
	
	for (auto [Card] : HandedCards.CardList)
	{
		if (Card == HandedCard.Card)
		{
			HandedCards.CardList.Remove(HandedCard);
			UsedCards.CardList.Add(HandedCard);
			break;
		}
	}
}

// 건내준 카드를 다시 사용된 카드 덱에 넣는다
void UBangCardManager::ReorderAvailCards(const FSingleCard HandedCard)
{
	if (HandedCards.CardList.Num() == 0) return;
	
	for (auto [Card] : HandedCards.CardList)
	{
		if (Card == HandedCard.Card)
		{
			HandedCards.CardList.Remove(HandedCard);
			AvailCards.CardList.Add(HandedCard);
			break;
		}
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
		
		FSingleCard SingleCard;
		SingleCard.Card = Card;
		
		AllCards.CardList.Add(SingleCard);
		CardDeckByType.FindOrAdd(Card->CardType).CardList.Add(SingleCard);
		switch (Card->CardType)
		{
			case ECardType::JobCard:
				{
					JobCards.CardList.Add(SingleCard);
					break;
				}
			case ECardType::ActiveCard:
				{
					ActiveCards.CardList.Add(SingleCard);
					break;
				}
			case ECardType::PassiveCard:
				{
					PassiveCards.CardList.Add(SingleCard);
					break;
				}
			case ECardType::CharacterCard:
				{
					CharacterCards.CardList.Add(SingleCard);
					break;
				}
		}
	}
}

// 인원에 맞는 직업카드 추출 로직
void UBangCardManager::GetJobByPlayer(const int PlayerCount, TArray<EJobType>& SelectedCards_)
{
	if (PlayerCount < 4 || PlayerCount > 7) return;

	int OfficerCount    = (PlayerCount >= 4) ? 1 : 0;
	int SubOfficerCount = (PlayerCount >= 5) ? (PlayerCount >= 7 ? 2 : 1) : 0;
	int OutlawCount     = (PlayerCount == 3) ? 1 : (PlayerCount == 4 || PlayerCount == 5) ? 2 : 3;
	int BetrayerCount   = 1;

	for (FSingleCard Card : JobCards.CardList)
	{
		if (!Card.Card) continue;

		if (Card.Card->CardType == ECardType::JobCard)
		{
			if (const UBangJobCard* JobCard = Cast<UBangJobCard>(Card.Card))
			{
				switch (JobCard->JobType)
				{
				case EJobType::Officer:
					{
						if (OfficerCount != 0)
						{
							SelectedCards_.Add(JobCard->JobType);
							OfficerCount--;
						}
						break;
					}
				case EJobType::SubOfficer:
					{
						if (SubOfficerCount != 0)
						{
							SelectedCards_.Add(JobCard->JobType);
							SubOfficerCount--;
						}
						break;
					}
				case EJobType::Outlaw:
					{
						if (OutlawCount != 0)
						{
							SelectedCards_.Add(JobCard->JobType);
							OutlawCount--;
						}
						break;
					}
				case EJobType::Betrayer:
					{
						if (BetrayerCount != 0)
						{
							SelectedCards_.Add(JobCard->JobType);
							BetrayerCount--;
						}
						break;
					}
				}
			}
		}
	}

	ShuffleArray(SelectedCards_);
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