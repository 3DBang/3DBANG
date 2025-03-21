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

	TObjectPtr<UBangInGameChattingWidget> GetChattingWidget() const;
	TObjectPtr<UBangInGamePlayerListWidget> GetPlayerListWidget() const { return PlayerListWidgetInstance; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBangInGameChattingWidget> ChattingWidgetClass;

	UPROPERTY()
	UBangInGameChattingWidget* ChattingWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBangInGamePlayerListWidget> PlayerListWidgetClass;

	UPROPERTY()
	UBangInGamePlayerListWidget* PlayerListWidgetInstance;
};
