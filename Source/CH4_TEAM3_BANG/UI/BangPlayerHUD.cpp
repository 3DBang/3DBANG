#include "BangPlayerHUD.h"

#include "BangInGameChattingWidget.h"
#include "Blueprint/UserWidget.h"
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

	if (APlayerController* PlayerController = GetOwningPlayerController()) {
		if (ABangPlayerController* BangPlayerController = Cast<ABangPlayerController>(PlayerController)) {
			BangPlayerController->NotifyHUDLoaded();
		}
	}
}
