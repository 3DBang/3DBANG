#include "BangPlayerHUD.h"

#include "Blueprint/UserWidget.h"
#include "Card/CardList.h"
#include "Card/TableCard.h"
#include "Chat/BangInGameChattingWidget.h"
#include "UI/Chat/PlayerListGameLog.h" 
#include "PlayerController/BangPlayerController.h"

void ABangPlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	APlayerController* PC = GetOwningPlayerController();

	if (ChattingWidgetClass)
	{
		ChattingWidgetInstance = CreateWidget<UBangInGameChattingWidget>(World, ChattingWidgetClass);
		if (ChattingWidgetInstance)
		{
			ChattingWidgetInstance->AddToViewport();
		}
	}

	if (CardListWidgetClass)
	{
		CardListWidgetInstance = CreateWidget<UCardList>(World, CardListWidgetClass);
		if (CardListWidgetInstance)
		{
			CardListWidgetInstance->AddToViewport();
		}
	}

	if (PlayerListGameLog)
	{
		PlayerListGameLogInstance = CreateWidget<UPlayerListGameLog>(World, PlayerListGameLog);
		if (PlayerListGameLogInstance)
		{
			PlayerListGameLogInstance->AddToViewport();
		}
	}

	if (ABangPlayerController* BangPC = Cast<ABangPlayerController>(PC))
	{
		BangPC->NotifyHUDLoaded();
	}
}

void ABangPlayerHUD::ShowDrawCardUI(const TArray<FSingleCard>& Cards)
{
	UE_LOG(LogTemp, Warning, TEXT("ShowDrawCardUI 호출됨! 카드 수: %d"), Cards.Num());

	if (!TableCardWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("TableCardWidgetClass is NULL"));
		return;
	}

	TableCardWidgetInstance = CreateWidget<UTableCard>(GetWorld(), TableCardWidgetClass);
	if (!TableCardWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("TableCardWidgetInstance 생성 실패"));
		return;
	}

	TableCardWidgetInstance->AddToViewport();
	TableCardWidgetInstance->InitializeCardList(Cards);
	UE_LOG(LogTemp, Warning, TEXT("TableCardWidgetInstance 생성 및 초기화 완료"));
}

void ABangPlayerHUD::SetupTurnCardSelection(ECardSelectPurpose Purpose, FText ButtonText, int32 NumCardsToSelect)
{
	if (!CardListWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerHUD::SetupTurnCardSelection] : CardListWidgetInstance is null"));
		return;
	}

	// 버튼 텍스트 설정
	CardListWidgetInstance->UseInputButtonText->SetText(ButtonText);
	//이넘과 선택할 카드 개수
	CardListWidgetInstance->CurrentCardSelectPurpose = Purpose;
	CardListWidgetInstance->CardsToSelectCount = NumCardsToSelect;
	
	CardListWidgetInstance->UseInputButton->SetVisibility(ESlateVisibility::Visible);
	UE_LOG(LogTemp, Error, TEXT("[ABangPlayerHUD::SetupTurnCardSelection] : 카드 개수 설정 %d"), NumCardsToSelect);
	if (Purpose == ECardSelectPurpose::UseCard)
	{
		CardListWidgetInstance->TurnEndButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CardListWidgetInstance->TurnEndButton->SetVisibility(ESlateVisibility::Hidden);
	}
	
	UE_LOG(LogTemp, Log, TEXT("SetupTurnCardSelection called for Purpose: %s, ButtonText: %s, CardsToSelectCount: %d"),	*UEnum::GetValueAsString(Purpose), *ButtonText.ToString(), NumCardsToSelect);
}
