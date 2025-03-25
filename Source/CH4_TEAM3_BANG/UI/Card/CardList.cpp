#include "CardList.h"

#include "Card.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

class USizeBoxSlot;
class UCanvasPanelSlot;

void UCardList::SelectCard(UCard* CardWidget)
{
	if (!CardWidget)
		return;

	FSingleCard CardData = CardWidget->Card;

	bool bAlreadySelected = false;
	int32 SelectedIndex = -1;

	// 이미 선택한 카드인지 확인
	for (int32 i = 0; i < SelectedCardWidgetList.Num(); ++i)
	{
		if (SelectedCardWidgetList[i] == CardWidget)
		{
			bAlreadySelected = true;
			SelectedIndex = i;
			break;
		}
	}

	//선택한 카드인경우
	if (bAlreadySelected)
	{
		// UE_LOG 부분 수정!
		if (CardWidget->Card.Card != nullptr && CardWidget->Card.Card->CardName.IsEmpty() == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("[UCardList::SelectCard] 이미 선택된 카드 다시 클릭, 선택 해제: %s"), *CardWidget->Card.Card->CardName.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[UCardList::SelectCard] CardWidget->Card.Card 또는 CardName이 유효하지 않습니다."));
		}
		CardWidget->SetCardSelected(false);

		SelectedCardWidgetList.RemoveAt(SelectedIndex);
		SelectedCardList.CardList.RemoveAt(SelectedIndex);

		// 카드 선택 델리게이트 브로드캐스트 (선택된 카드 리스트 전체를 전달)
		OnCardSelected.Broadcast(FSingleCard());
		return;
	}
	else
	{
		// 새 카드 선택 시 선택 가능한 개수 확인
		if (CardsToSelectCount > 0 && SelectedCardList.CardList.Num() >= CardsToSelectCount)
		{
			// 선택 가능한 카드 개수 초과: 가장 오래된 카드 제거하고 새 카드 선택

			// 1. 가장 오래된 카드 가져오기 (리스트의 첫 번째 요소)
			UCard* OldestCardWidget = SelectedCardWidgetList[0];

			// 2. 가장 오래된 카드 선택 해제 (UI에서, 그리고 리스트에서 제거)
			if (OldestCardWidget)
			{
				OldestCardWidget->SetCardSelected(false); // UI에서 선택 해제
			}
			SelectedCardWidgetList.RemoveAt(0);
			SelectedCardList.CardList.RemoveAt(0);

			// 3. 새로 선택한 카드 추가 (UI에서 선택, 그리고 리스트에 추가)
			SelectedCardWidgetList.Add(CardWidget);
			SelectedCardList.CardList.Add(CardData);
			CardWidget->SetCardSelected(true); // UI에서 선택

			UE_LOG(LogTemp, Warning, TEXT("[UCardList::SelectCard] 선택 가능한 카드 개수 초과 (%d), 가장 오래된 카드 제거 후 새 카드 선택: %s"), CardsToSelectCount, *CardWidget->GetName());
		}
		else
		{
			// 선택 가능한 경우 (개수 제한 내) - 기존 로직 유지
			UE_LOG(LogTemp, Warning, TEXT("[UCardList::SelectCard] 카드 선택 시작: %s"), *CardWidget->GetName());

			SelectedCardWidgetList.Add(CardWidget); // 선택된 위젯 리스트에 추가
			SelectedCardList.CardList.Add(CardData);        // 선택된 카드 데이터 리스트에 추가
			CardWidget->SetCardSelected(true); // UI에서 선택

			// 카드 선택 델리게이트 브로드캐스트 (선택된 카드 리스트 전체를 전달)
			OnCardSelected.Broadcast(FSingleCard());
		}
	}
}

UCard* UCardList::AddCard(FSingleCard CardData)
{
	if (!CardWidgetClass || !ScrollBox)
		return nullptr;
    
	// 새 카드 생성
	UCard* NewCardWidget = CreateWidget<UCard>(this, CardWidgetClass);
	if (!NewCardWidget)
	{
		return nullptr;
	}
    
	// 카드 데이터 설정
	NewCardWidget->InitializeWithCard(CardData);
    
	// 클릭 이벤트 바인딩
	NewCardWidget->OnCardClicked.AddDynamic(this, &UCardList::SelectCard);
    
	// 스크롤 박스에 추가
	ScrollBox->AddChild(NewCardWidget);
    
	return NewCardWidget;
}

void UCardList::AddCardToCharacterCardSlot(FSingleCard CardData)
{
	CharacterCardSlot->InitializeWithCard(CardData);
}

void UCardList::AddCardToJobCardSlot(FSingleCard CardData)
{
	JobCardSlot->InitializeWithCard(CardData);
}

void UCardList::OnUseInputButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[UCardList::OnUseInputButtonClicked] Use Card Button 클릭!"));
	if (SelectedCardList.CardList.Num() == CardsToSelectCount)
	{
		
		// 선택된 카드들을 로그로 출력
		for (const FSingleCard& Card : SelectedCardList.CardList)
		{
			if (Card.Card)
			{
				UE_LOG(LogTemp, Log, TEXT("[UCardList::OnUseInputButtonClicked] 선택된 카드: %s, SymbolType: %s, SymbolNumber: %d"),
					*Card.Card->GetName(),
					*UEnum::GetValueAsString(Card.Card->SymbolType),
					Card.Card->SymbolNumber);
			}
		}
		// 선택된 카드 리스트를 델리게이트로 브로드캐스트 (리스트 전체 전달)
		OnUseCard.Broadcast(SelectedCardList, CurrentCardSelectPurpose);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[UCardList::OnUseInputButtonClicked] 선택할 카드가 부족합니다"));
		// 선택된 카드가 없을 때의 처리 (예: 경고 메시지 표시)
	}
}

void UCardList::OnHiddenCardListButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[UCardList::OnHiddenCardListButtonClicked] Hidden CardList Button 클릭!"));
	bIsHidden = !bIsHidden;
	if(bIsHidden)
	{
		ScrollBox->SetVisibility(ESlateVisibility::Hidden);
		HiddenCardListButtonText->SetText(FText::FromString(TEXT("카드 보이기")));
	}
	else
	{
		ScrollBox->SetVisibility(ESlateVisibility::Visible);
		HiddenCardListButtonText->SetText(FText::FromString(TEXT("카드 숨기기")));
	}
}

void UCardList::OnTurnEndButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[UCardList::OnTurnEndButtonClicked] Turn End Button 클릭!"));
}

void UCardList::ClearCards()
{
	if (!ScrollBox) return;

	ScrollBox->ClearChildren(); // ScrollBox의 모든 자식 위젯(카드) 제거
	JobCardSlot->Border->SetBrushFromTexture(nullptr);
	CharacterCardSlot->Border->SetBrushFromTexture(nullptr);
	
	// 선택된 카드 정보 초기화 
	SelectedCardWidgetList.Empty();
	SelectedCardList.CardList.Empty();
	
}

void UCardList::RemoveSelectedCard(FSingleCard RemoveCard)
{
	if (!ScrollBox || ScrollBox->GetChildrenCount() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UCardList::RemoveSelectedCard] ScrollBox가 비어있거나 유효하지 않습니다."));
		return;
	}

	bool bCardRemoved = false; // 카드 제거 여부를 추적하는 변수
	
	for (int32 i = 0; i < ScrollBox->GetChildrenCount(); ++i)
	{
		UCard* CardWidget = Cast<UCard>(ScrollBox->GetChildAt(i));
		if (CardWidget && CardWidget->Card.Card)
		{
			if (CardWidget->Card.Card->SymbolType == RemoveCard.Card->SymbolType &&
				CardWidget->Card.Card->SymbolNumber == RemoveCard.Card->SymbolNumber)
			{
				UE_LOG(LogTemp, Log, TEXT("[UCardList::RemoveSelectedCard] 카드 제거: %s, Card Symbol: %s, Number: %d"),
					*CardWidget->GetName(), *UEnum::GetValueAsString(RemoveCard.Card->SymbolType), RemoveCard.Card->SymbolNumber);

				ScrollBox->RemoveChildAt(i);
				CardWidget->RemoveFromParent();
				bCardRemoved = true;
				break;
			}
		}
	}

	if (bCardRemoved)
	{
		// 제거된 카드가 현재 선택된 카드였다면 선택 정보 초기화 (리스트에서 제거)
		for (int32 i = 0; i < SelectedCardWidgetList.Num(); ++i)
		{
			if (SelectedCardWidgetList[i] && SelectedCardWidgetList[i]->Card.Card &&
				SelectedCardWidgetList[i]->Card.Card->SymbolType == RemoveCard.Card->SymbolType &&
				SelectedCardWidgetList[i]->Card.Card->SymbolNumber == RemoveCard.Card->SymbolNumber)
			{
				SelectedCardWidgetList[i]->SetCardSelected(false);
				SelectedCardWidgetList.RemoveAt(i);
				SelectedCardList.CardList.RemoveAt(i);
				break; // 리스트에서 제거 후 종료 (중복 제거 방지)
			}
		}

		// 카드 선택 상태 변경 알림 (선택 해제)
		OnCardSelected.Broadcast(FSingleCard()); // 이전처럼 단일 카드 정보가 아닌 빈 FSingleCard 브로드캐스트, 필요에 따라 수정
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[UCardList::RemoveSelectedCard] 제거할 카드 (%s %d) 를 찾지 못했습니다."),
			*UEnum::GetValueAsString(RemoveCard.Card->SymbolType), RemoveCard.Card->SymbolNumber);
	}
}

void UCardList::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!CanvasPanel || !SizeBox || !ScrollBox)
	{
		UE_LOG(LogTemp, Error, TEXT(" UCardList::NativeConstruct : 위젯 바인딩 실패"));
		return;
	}
	
	bIsHidden = true;
	ScrollBox->SetVisibility(ESlateVisibility::Hidden);
	HiddenCardListButtonText->SetText(FText::FromString(TEXT("카드 보이기")));
	UseInputButton->SetVisibility(ESlateVisibility::Visible);
	TurnEndButton->SetVisibility(ESlateVisibility::Hidden);
	
	UseInputButton->OnClicked.AddDynamic(this, &UCardList::OnUseInputButtonClicked);
	HiddenCardListButton->OnClicked.AddDynamic(this, &UCardList::OnHiddenCardListButtonClicked);
	TurnEndButton->OnClicked.AddDynamic(this, &UCardList::OnTurnEndButtonClicked);
	CardsToSelectCount = 3;
	
	for (int32 i = 0; i < ScrollBox->GetChildrenCount(); i++)
	{
		UCard* ExistingCard = Cast<UCard>(ScrollBox->GetChildAt(i));
		if (ExistingCard)
		{
			// 이벤트 바인딩
			ExistingCard->OnCardClicked.AddDynamic(this, &UCardList::SelectCard);
		}
	}
}
