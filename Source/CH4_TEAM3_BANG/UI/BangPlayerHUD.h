#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BangPlayerHUD.generated.h"

class UBangInGamePlayerListWidget;
class UBangInGameChattingWidget;

UCLASS()
class CH4_TEAM3_BANG_API ABangPlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<UBangInGameChattingWidget> ChattingWidgetInstance;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBangInGameChattingWidget> ChattingWidgetClass;
};
