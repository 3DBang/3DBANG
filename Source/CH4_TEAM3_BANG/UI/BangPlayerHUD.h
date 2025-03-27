#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Card/BangCardManager.h"
#include "BangPlayerHUD.generated.h"

class UBangInfoWidget;
class UCardList;
class UTableCard;
class UBangInGameChattingWidget;
class UPlayerListGameLog; 

UCLASS()
class CH4_TEAM3_BANG_API ABangPlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	ABangPlayerHUD();
	
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<UBangInGameChattingWidget> ChattingWidgetInstance;

	UPROPERTY()
	TObjectPtr<UCardList> CardListWidgetInstance;

	UPROPERTY()
	TObjectPtr<UTableCard> TableCardWidgetInstance;

	UPROPERTY()
	TObjectPtr<UPlayerListGameLog> PlayerListGameLogInstance; //  새 위젯 인스턴스 추가

	UPROPERTY()
	TObjectPtr<UBangInfoWidget> PlayerInfoWidgetInstance;

	void ShowDrawCardUI(const FCardCollection& Cards);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetupTurnCardSelection(ECardSelectPurpose Purpose = ECardSelectPurpose::UseCard, FText ButtonText = INVTEXT("사용하기"), int32 NumCardsToSelect = 1);

	UFUNCTION()
	void ShowBangInfoWidget(uint32 TargetPlayerUniqueID, bool bShowButtonWidget);
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

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBangInfoWidget> PlayerInfoWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FVector2D WidgetOffset;
};
