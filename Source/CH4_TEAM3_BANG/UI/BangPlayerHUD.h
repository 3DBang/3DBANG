#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Card/BangCardManager.h"
#include "BangPlayerHUD.generated.h"

class UCardList;
class UTableCard;
class UBangInGameChattingWidget;
class UPlayerListGameLog; 

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

	UPROPERTY()
	TObjectPtr<UPlayerListGameLog> PlayerListGameLogInstance; //  새 위젯 인스턴스 추가

	void ShowDrawCardUI(const TArray<FSingleCard>& Cards);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetupTurnCardSelection(ECardSelectPurpose Purpose = ECardSelectPurpose::UseCard, FText ButtonText = INVTEXT("사용하기"), int32 NumCardsToSelect = 1);
	
protected:
	// === 클래스 설정들 ===
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBangInGameChattingWidget> ChattingWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCardList> CardListWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTableCard> TableCardWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPlayerListGameLog> PlayerListGameLog; //  새 위젯 클래스 추가
};
