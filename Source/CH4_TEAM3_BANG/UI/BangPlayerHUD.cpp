#include "BangPlayerHUD.h"

#include "Blueprint/UserWidget.h"
#include "Card/CardList.h"
#include "Card/TableCard.h"         // UTableCard

#include "Chat/BangInGameChattingWidget.h"
#include "PlayerController/BangPlayerController.h"

void ABangPlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	// 채팅 UI 노출
	if (ChattingWidgetClass)
	{
		ChattingWidgetInstance = CreateWidget<UBangInGameChattingWidget>(GetWorld(), ChattingWidgetClass);
		if (ChattingWidgetInstance)
		{
			ChattingWidgetInstance->AddToViewport();
		}
	}

	if (CardListWidgetClass)
	{
		CardListWidgetInstance = CreateWidget<UCardList>(GetWorld(), CardListWidgetClass);
		if (CardListWidgetInstance)
		{
			CardListWidgetInstance->AddToViewport();
		}
	}
	
	if (APlayerController* PlayerController = GetOwningPlayerController()) {
		if (ABangPlayerController* BangPlayerController = Cast<ABangPlayerController>(PlayerController)) {
			BangPlayerController->NotifyHUDLoaded();
		}
	}
}

void ABangPlayerHUD::ShowDrawCardUI(const TArray<FSingleCard>& Cards)
{
	if (!TableCardWidgetClass) return;

	TableCardWidgetInstance = CreateWidget<UTableCard>(GetWorld(), TableCardWidgetClass);
	if (TableCardWidgetInstance)
	{
		TableCardWidgetInstance->AddToViewport();
		TableCardWidgetInstance->InitializeCardList(Cards); // 카드 UI 생성 함수
	}
}