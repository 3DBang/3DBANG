#include "BangPlayerHUD.h"

#include "BangInGameChattingWidget.h"
#include "BangInGamePlayerListWidget.h"
#include "Blueprint/UserWidget.h"

void ABangPlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Error, TEXT("BangPlayerHUD::BeginPlay"));

	// 채팅 UI 노출
	if (ChattingWidgetClass)
	{
		ChattingWidgetInstance = CreateWidget<UBangInGameChattingWidget>(GetWorld(), ChattingWidgetClass);
		if (ChattingWidgetInstance)
		{
			//ChattingWidgetInstance->AddToViewport();
		}
	}

	// 플레이어 목록 노출
	if (PlayerListWidgetClass)
	{
		PlayerListWidgetInstance = CreateWidget<UBangInGamePlayerListWidget>(GetWorld(), PlayerListWidgetClass);
		if (PlayerListWidgetInstance)
		{
			//PlayerListWidgetInstance->AddToViewport();
		}
	}
}
