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
	
	
	if (SelectedCardWidget == CardWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UCardList::SelectCard] 이미 선택된 카드 다시 클릭, 선택 해제: %s"), *CardWidget->GetName());
		SelectedCardWidget->SetCardSelected(false); 
		SelectedCardWidget = nullptr;             
		SelectedCard = FSingleCard();            
		
		// 빈 FSingleCard 또는 유효하지 않은 값으로 브로드캐스트
		OnCardSelected.Broadcast(FSingleCard()); 
		
		return; 
	}
	
	// 이전에 선택된 카드가 있으면 선택 취소
	if (SelectedCardWidget && SelectedCardWidget != CardWidget)
	{
		SelectedCardWidget->SetCardSelected(false);
	}

	UE_LOG(LogTemp, Warning, TEXT("[UCardList::SelectCard] 카드 선택 시작: %s"), *CardWidget->GetName());

	// 새 카드 선택
	SelectedCardWidget = CardWidget;
	SelectedCard = CardWidget->Card;
	CardWidget->SetCardSelected(true);
    
	// 델리게이트로 선택 이벤트 알림
	OnCardSelected.Broadcast(SelectedCard);
	
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
	//case 버리기 = OnCardSelectionComplete(유저가 선택한 카드, 버리는 목적)
	OnUseCard.Broadcast(SelectedCard);
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
	
	// 선택된 카드 정보 초기화 (선택 해제)
	SelectedCardWidget = nullptr;
	SelectedCard = FSingleCard();
	
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
		if (CardWidget && CardWidget->Card.Card) // CardWidget이 유효하고 Card 데이터가 있는지 확인
		{
			// 카드 심볼 비교
			if (CardWidget->Card.Card->SymbolType == RemoveCard.Card->SymbolType &&
				CardWidget->Card.Card->SymbolNumber == RemoveCard.Card->SymbolNumber) 
			{
				UE_LOG(LogTemp, Log, TEXT("[UCardList::RemoveSelectedCard] 카드 제거: %s, Card Symbol: %s, Number: %d"),
					*CardWidget->GetName(), *UEnum::GetValueAsString(RemoveCard.Card->SymbolType), RemoveCard.Card->SymbolNumber);
				
				ScrollBox->RemoveChildAt(i);
				
				CardWidget->RemoveFromParent();

				bCardRemoved = true; // 카드 제거 성공 표시
				break; // 카드 제거 후 루프 종료 (중복 제거 방지)
			}
		}
	}

	if (bCardRemoved)
	{
		// 5. 제거된 카드가 현재 선택된 카드였다면 선택 정보 초기화
		if (SelectedCardWidget && SelectedCardWidget->Card.Card &&
			SelectedCardWidget->Card.Card->SymbolType == RemoveCard.Card->SymbolType &&
			SelectedCardWidget->Card.Card->SymbolNumber == RemoveCard.Card->SymbolNumber)
		{
			SelectedCardWidget->SetCardSelected(false);
			SelectedCardWidget = nullptr;
			SelectedCard = FSingleCard();

			// 6. 카드 선택 상태 변경 알림 (선택 해제)
			OnCardSelected.Broadcast(FSingleCard());
		}
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
	UseInputButton->SetVisibility(ESlateVisibility::Hidden);
	TurnEndButton->SetVisibility(ESlateVisibility::Hidden);
	
	UseInputButton->OnClicked.AddDynamic(this, &UCardList::OnUseInputButtonClicked);
	HiddenCardListButton->OnClicked.AddDynamic(this, &UCardList::OnHiddenCardListButtonClicked);
	TurnEndButton->OnClicked.AddDynamic(this, &UCardList::OnTurnEndButtonClicked);

	
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
