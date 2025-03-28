// Fill out your copyright notice in the Description page of Project Settings.


#include "RobberyChoiceWidget.h"
#include "Card.h"
#include "Components/WrapBox.h"

void URobberyChoiceWidget::InitializeRobberyCardList(const FCardCollection& FrontCards, const FCardCollection& BackCards,
	const ECardSelectPurpose _CardSelectPurpose)
{
	if (!CardContainer || !CardWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UTableCard: CardContainer 또는 CardWidgetClass가 없습니다."));
		return;
	}

	CardSelectPurpose = _CardSelectPurpose;

	// 혹시 기존에 있던 카드 제거
	CardContainer->ClearChildren();
	
	for (const FSingleCard& CardData : FrontCards.CardList)
	{
		if (UCard* CardWidget = CreateWidget<UCard>(GetWorld(), CardWidgetClass))
		{
			CardWidget->InitializeWithCard(CardData);
			CardContainer->AddChildToWrapBox(CardWidget);
	
			// 카드를 클릭했을때 HandleCardClicked 함수로 브로드 케스트
			CardWidget->OnCardClicked.AddDynamic(this, &UTableCard::HandleCardClicked);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UTableCard: 앞면 카드 위젯 생성 실패"));
		}
	}
	
	for (const FSingleCard& CardData : BackCards.CardList)
	{
		if (UCard* CardWidget = CreateWidget<UCard>(GetWorld(), CardWidgetClass))
		{
			CardWidget->InitializeWithCard(CardData);
			CardContainer->AddChildToWrapBox(CardWidget);

			if (IsValid(BackCardIcon))
			CardWidget->SetCardImage(BackCardIcon);
	
			// 카드를 클릭했을때 HandleCardClicked 함수로 브로드 케스트
			CardWidget->OnCardClicked.AddDynamic(this, &UTableCard::HandleCardClicked);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UTableCard: 뒷면 카드 위젯 생성 실패"));
		}
	}
}
