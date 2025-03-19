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
