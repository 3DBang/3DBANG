#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Card/BangCardManager.h"
#include "BangPlayerHUD.generated.h"

class UCardList;
class UTableCard;
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

	UPROPERTY()
	TObjectPtr<UCardList> CardListWidgetInstance;

	UPROPERTY()
	TObjectPtr<UTableCard> TableCardWidgetInstance;
	void ShowDrawCardUI(const TArray<FSingleCard>& Cards);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBangInGameChattingWidget> ChattingWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCardList> CardListWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTableCard> TableCardWidgetClass;


};
