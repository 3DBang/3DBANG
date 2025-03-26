#include "UI/PlayerInfo/BangInfoWidget.h"

#include "Card/BangCardManager.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "PlayerState/BangPlayerState.h"
#include "UI/Card/Card.h"

void UBangInfoWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!PlayerNameText || !CharacterCard  || !TurnStatusText || !CardCountText || !UseCardButton)
    {
        UE_LOG(LogTemp, Error, TEXT("[UBangInfoWidget::NativeConstruct] : 바인딩 안됨")); // UBangInfoWidget: 텍스트 블록 위젯이 제대로 바인딩되지 않았습니다!
        return;
    }

    UseCardButton->OnClicked.AddDynamic(this, &UBangInfoWidget::OnUseCardButtonClicked);
}

//마우스 나갈때 위젯 사라지게 
void UBangInfoWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    TargetPlayerUniqueID = 0;
    this->SetVisibility(ESlateVisibility::Hidden);
}

// 버튼을 누르면 플레이어의 아이디값을 반환
void UBangInfoWidget::OnUseCardButtonClicked()
{
    
    PlayerSelectedDelegate.Broadcast(TargetPlayerUniqueID);
}

void UBangInfoWidget::ShowPlayerInfo(uint32 _TargetPlayerUniqueID)
{
    if (!PlayerNameText || !CardCountText || !TurnStatusText || !ScrollBox || !CharacterCard) // CharacterCard 체크 추가
    {
        UE_LOG(LogTemp, Error, TEXT("[UBangInfoWidget::ShowPlayerInfo] : 위젯 바인딩 에러"));
        return;
    }

    
    TargetPlayerUniqueID = _TargetPlayerUniqueID;
    FPlayerInformation* PlayerInfo= GetTargetPlayerInfo(_TargetPlayerUniqueID);
    
    if (!PlayerInfo)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UBangInfoWidget::ShowPlayerInfo] : PlayerInfo is null"));
        return;
    }

    FSingleCard PlayerCharacterCard;
    GetBangPlayerState()->GetCardByCharacter(PlayerInfo->CharacterCardType, PlayerCharacterCard);
    CharacterCard->InitializeWithCard(PlayerCharacterCard);
    PlayerNameText->SetText(FText::FromString(PlayerInfo->PlayerName));
    CardCountText->SetText(FText::FromString(FString::Printf(TEXT("보유 카드 수: %d"), PlayerInfo->MyCards.PlayerCards.Num())));

    if (PlayerInfo->bIsMyTurn)
    {
        TurnStatusText->SetVisibility(ESlateVisibility::Visible);
        TurnStatusText->SetText(FText::FromString(TEXT("플레이어 턴")));
    }
    else
    {
        TurnStatusText->SetVisibility(ESlateVisibility::Hidden);
    }

    ScrollBox->ClearChildren();

    // 장착카드를 돌면서 위젯 생성 및 집어 넢기
    for (const auto& CardSymbol : PlayerInfo->EquippedCards.PlayerCards)
    {
        
        UCard* CardWidget = CreateWidget<UCard>(this, CardWidgetClass);
        if (CardWidget)
        {
           // GetBangPlayerState();
            //초기화
            //CardWidget->InitializeWithCard(싱글 카드 정보 필요);
            ScrollBox->AddChild(CardWidget);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[UBangInfoWidget::UpdateInfo] : CardWidget 생성 실패"));
        }
    }
}

 FPlayerInformation* UBangInfoWidget::GetTargetPlayerInfo(uint32 _TargetPlayerUniqueID)
{
    if(ABangPlayerState* BangPlayerState= GetBangPlayerState())
    {
        return BangPlayerState->PlayerInfo.GetPlayerInformation(_TargetPlayerUniqueID);
    }
    return nullptr;
}


ABangPlayerState* UBangInfoWidget::GetBangPlayerState() const
{
    APlayerController* OwningPlayerController = GetOwningPlayer();
    if (OwningPlayerController)
    {
        ABangPlayerState* PlayerState = OwningPlayerController->GetPlayerState<ABangPlayerState>();
        if (PlayerState)
        {
            return PlayerState;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[UBangInfoWidget::GetBangPlayerState] : PlayerState is null"));
            return nullptr;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[UBangInfoWidget::GetBangPlayerState] : OwningPlayerController is null"));
    }

    return nullptr;
}