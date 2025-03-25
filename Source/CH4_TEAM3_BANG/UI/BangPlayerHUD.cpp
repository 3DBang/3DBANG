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
	if (!TableCardWidgetClass) return;

	TableCardWidgetInstance = CreateWidget<UTableCard>(GetWorld(), TableCardWidgetClass);
	if (TableCardWidgetInstance)
	{
		TableCardWidgetInstance->AddToViewport();
		TableCardWidgetInstance->InitializeCardList(Cards);
	}
}
