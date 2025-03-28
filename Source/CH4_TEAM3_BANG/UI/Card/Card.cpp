#include "Card.h"

#include "CardDescriptionWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Card/BangCardManager.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "TimerManager.h"


void UCard::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 위젯 바인딩 확인 (디버깅 용도)
	if (!SizeBox || !Border || !CardHighlights)
	{
		UE_LOG(LogTemp, Error, TEXT("UMG Widget Binding Failed in UCard!"));
		return; // 바인딩 실패 시 추가 로직 실행 방지
	}
	
	// Border 위젯에 클릭 이벤트 바인딩
	Border->OnMouseButtonDownEvent.BindUFunction(this, FName("OnBorderClicked"));

	//현재 선택 되었는지
	bIsSelected = false;
	// 초기 하이라이트 이미지 숨김
	CardHighlights->SetVisibility(ESlateVisibility::Hidden);
	CardHoverHighlights->SetVisibility(ESlateVisibility::Hidden);
}

void UCard::BeginDestroy()
{
	Super::BeginDestroy();

	if (DescriptionWidgetInstance)
	{
		DescriptionWidgetInstance->RemoveFromParent();
		DescriptionWidgetInstance = nullptr;
	}
}

void UCard::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	CardHoverHighlights->SetVisibility(ESlateVisibility::Visible);
	// 1.3초 딜레이 타이머 시작, 타이머 만료 시 ShowDescriptionWidgetDelayed 함수 호출
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("ShowDescriptionWidgetDelayed")); // 딜레이 후 호출할 함수 지정 (ShowDescriptionWidgetDelayed)
	GetWorld()->GetTimerManager().SetTimer(DescriptionDelayTimerHandle, TimerDelegate, 1.1f, false); // 1.3초 딜레이, 루프 X (false)
}

void UCard::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	CardHoverHighlights->SetVisibility(ESlateVisibility::Hidden);
	if (DescriptionWidgetInstance)
	{
		DescriptionWidgetInstance->SetVisibility(ESlateVisibility::Hidden); // 설명 위젯 숨김
	}

	GetWorld()->GetTimerManager().ClearTimer(DescriptionDelayTimerHandle);
}

void UCard::InitializeWithCard(const FSingleCard& InCard)
{
	Card = InCard;
	UpdateCardDisplay();
}

void UCard::OnBorderClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UE_LOG(LogTemp, Log, TEXT("[UCard::OnBorderClicked] 카드 클릭됨: %s"), *GetName()); 
	OnCardClicked.Broadcast(this);
}

void UCard::ToggleCardSelection()
{
	SetCardSelected(!bIsSelected); // 현재 선택 상태의 반대로 설정
}

void UCard::SetCardSelected(bool bSelected)
{
	bIsSelected = bSelected;
	UE_LOG(LogTemp, Log, TEXT("[UCard::SetCardSelected] 카드 선택 상태 변경: %s, Selected = %s"), *GetName(), bSelected ? TEXT("true") : TEXT("false")); // 로그 추가
	UpdateCardHighlightVisibility(); // 선택 상태 변경에 따라 하이라이트 이미지 visibility 업데이트
}

void UCard::UpdateCardHighlightVisibility()
{
	UE_LOG(LogTemp, Log, TEXT("[UCard::UpdateCardHighlightVisibility] 하이라이트 Visibility 업데이트: %s, Selected = %s"), *GetName(), bIsSelected ? TEXT("true") : TEXT("false")); // 로그 추가
	if (bIsSelected)
	{
		CardHighlights->SetVisibility(ESlateVisibility::Visible); // 선택 시 하이라이트 이미지 표시
	}
	else
	{
		CardHighlights->SetVisibility(ESlateVisibility::Hidden);  // 선택 해제 시 하이라이트 이미지 숨김
	}
}

void UCard::UpdateCardDisplay()
{
	
	// Border 위젯에 카드가 가지고 있는 이미지를 설정
	if (Card.Card && Card.Card->CardIcon)
	{
     	UE_LOG(LogTemp, Log, TEXT("[UCard::UpdateCardDisplay] 카드 디스플레이 업데이트: CardName = %s, CardDes: %s"), *Card.Card->CardName.ToString(), *Card.Card->CardDescription.ToString());
		Border->SetVisibility(ESlateVisibility::Visible);
		// Border의 Brush를 CardIcon으로 설정
		Border->SetBrushFromTexture(Card.Card->CardIcon);
		UE_LOG(LogTemp, Log, TEXT("[UCard::UpdateCardDisplay] Border 이미지를 카드 아이콘으로 설정: %s"), *Card.Card->CardName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[UCard::UpdateCardDisplay] 카드 아이콘이 설정되지 않음"));
		Border->SetVisibility(ESlateVisibility::Hidden);
		Border->SetBrushFromTexture(nullptr);
	}
	
}

void UCard::SetCardImage(UTexture2D* InImage)
{
	Border->SetBrushFromTexture(InImage);
}

void UCard::ShowDescriptionWidgetDelayed()
{
	 if (!DescriptionWidgetInstance) // DescriptionWidgetInstance 로 변경
    {
        if (DescriptionWidgetClass)
        {
            // 설명 위젯 생성 및 위치 설정 코드는 이전 답변과 동일 (NativeOnMouseEnter 에서 복사)
            DescriptionWidgetInstance = CreateWidget<UCardDescriptionWidget>(this, DescriptionWidgetClass); // DescriptionWidgetInstance 로 변경
            if (DescriptionWidgetInstance)
            {
                 if (!Card.Card)
                {
                    DescriptionWidgetInstance->SetDescriptionText(FText::FromString(TEXT("No Card Data")), FText::FromString(TEXT("No Card Data")));
                }
                else
                {
                    DescriptionWidgetInstance->SetDescriptionText(Card.Card->CardName,Card.Card->CardDescription);
                }


                UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(DescriptionWidgetInstance); 
                if (CanvasSlot)
                {
                    CanvasSlot->SetAnchors(FAnchors(1.f, 1.f));
                    CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
                	
                    FVector2D ViewportPosition;
                    UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
                    if (ViewportClient)
                    {
                    	//마우스 포인트 위치
                        FIntPoint PixelPosition;
                        ViewportClient->Viewport->GetMousePos(PixelPosition);
                        ViewportPosition.X = PixelPosition.X;
                        ViewportPosition.Y = PixelPosition.Y;
                    }
                	//마우스 포인트에서 20 추가
                    CanvasSlot->SetPosition(ViewportPosition + FVector2D(20, 20));
                }
                DescriptionCanvas->AddChild(DescriptionWidgetInstance); 
            }
        }
         else
        {
            UE_LOG(LogTemp, Log, TEXT("[UCard::ShowDescriptionWidgetDelayed] DescriptionWidgetClass is Null"));
        }
    }
     else
    {
        DescriptionWidgetInstance->SetVisibility(ESlateVisibility::Visible); // 이미 생성된 경우 visibility 만 변경
    }
}
