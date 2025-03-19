#include "BangCardManager.h"

#include "Card/BaseCard/BangCardBase.h"
#include "BangCardDataAsset.h"

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

void GetAllCards()
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

// 덱 섞기
void UBangCardManager::ShuffleDeck()
{
	if (AllCards.Num() == 0) return;

	
}

// 카드 타입별 정리
void UBangCardManager::CategorizeCardsByType()
{
	CardDeckByType.Empty();

	
}
