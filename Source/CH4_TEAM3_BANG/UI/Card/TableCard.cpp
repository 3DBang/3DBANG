

#include "UI/Card/TableCard.h"
#include "UI/Card/CardList.h"

void UTableCard::InitializeCardList(const TArray<FSingleCard>& Cards)
{
	if (!CardListWidget) return;

	CardListWidget->ClearCards();

	for (const FSingleCard& Card : Cards)
	{
		CardListWidget->AddCard(Card);
	}
}