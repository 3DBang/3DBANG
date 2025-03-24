#include "CardList.h"

#include "Card.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/ScrollBox.h"

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

		if (SelectedCardSlot)
		{
			SelectedCardSlot->InitializeWithCard(FSingleCard()); // 빈 FSingleCard로 초기화
		}
		
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
	
	//선택된 카드로 변경
	if (SelectedCardSlot)
	{
		SelectedCardSlot->InitializeWithCard(SelectedCard);
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

void UCardList::OnUseCardButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[UCardList::OnUseCardButtonClicked] Use Card Button 클릭!"));
	RemoveSelectedCard();
}

void UCardList::ClearCards()
{
	if (!ScrollBox) return;

	ScrollBox->ClearChildren(); // ScrollBox의 모든 자식 위젯(카드) 제거

	// 선택된 카드 정보 초기화 (선택 해제)
	SelectedCardWidget = nullptr;
	SelectedCard = FSingleCard();

	// SelectedCardSlot 초기화 (빈 카드 정보 표시)
	if (SelectedCardSlot)
	{
		SelectedCardSlot->InitializeWithCard(FSingleCard()); // 빈 FSingleCard로 초기화
	}
}

void UCardList::RemoveSelectedCard()
{
	if (SelectedCardWidget)
	{
		UE_LOG(LogTemp, Log, TEXT("[UCardList::RemoveSelectedCard] Removing Selected Card: %s"), *SelectedCardWidget->GetName());


		// 컨트롤러에서 카드를 사용할때 호출될 함수

		//

		// 스크롤 박스에서 카드 위젯 제거
		ScrollBox->RemoveChild(SelectedCardWidget);

		// 선택된 카드 정보 초기화
		SelectedCardWidget = nullptr;
		SelectedCard = FSingleCard();

		// 카드 선택 상태 변경 알림 (선택 해제)
		OnCardSelected.Broadcast(FSingleCard()); // 빈 FSingleCard 브로드캐스트하여 선택 해제 알림

		// SelectedCardSlot 초기화 (빈 카드 정보 표시)
		if (SelectedCardSlot)
		{
			SelectedCardSlot->InitializeWithCard(FSingleCard()); // 빈 FSingleCard로 초기화
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[UCardList::RemoveSelectedCard] No card selected to remove."));
	}
}

void UCardList::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!CanvasPanel || !SizeBox || !ScrollBox || !SelectedCardSlot)
	{
		UE_LOG(LogTemp, Error, TEXT(" UCardList::NativeConstruct : 위젯 바인딩 실패"));
		return;
	}

	UseCardButton->OnClicked.AddDynamic(this, &UCardList::OnUseCardButtonClicked);
	
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
