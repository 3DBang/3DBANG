// TableCard.cpp

#include "TableCard.h"
#include "Card.h"
#include "Components/WrapBox.h"

void UTableCard::InitializeCardList(const FCardCollection& Cards)
{
	if (!CardContainer || !CardWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UTableCard: CardContainer 또는 CardWidgetClass가 없습니다."));
		return;
	}

	// 혹시 기존에 있던 카드 제거
	CardContainer->ClearChildren();

	for (const FSingleCard& CardData : Cards.CardList)
	{
		UCard* CardWidget = CreateWidget<UCard>(GetWorld(), CardWidgetClass);
		if (CardWidget)
		{
			CardWidget->InitializeWithCard(CardData);
			CardContainer->AddChildToWrapBox(CardWidget);

			// 카드를 클릭했을때 HandleCardClicked 함수로 브로드 케스트
			CardWidget->OnCardClicked.AddDynamic(this, &UTableCard::HandleCardClicked);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UTableCard: 카드 위젯 생성 실패"));
		}
	}
}

void UTableCard::HandleCardClicked(UCard* ClickedCard)
{
	FCardCollection SelectedCards;
	SelectedCards.CardList.Add(ClickedCard->Card);
	TableCardClickedDelegate.Broadcast(SelectedCards, CardSelectPurpose);
}
