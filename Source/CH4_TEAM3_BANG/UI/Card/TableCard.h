// TableCard.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Card/BangCardManager.h"
#include "TableCard.generated.h"

class ABangPlayerController;
class UWrapBox;
class UCard;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTableCardClickedDelegate, FCardCollection, SelectedCards, ECardSelectPurpose, Purpose);

UCLASS()
class CH4_TEAM3_BANG_API UTableCard : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitializeCardList(const FCardCollection& Cards);

	FOnTableCardClickedDelegate TableCardClickedDelegate;

protected:
	UPROPERTY(meta = (BindWidget))
	UWrapBox* CardContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card")
	TSubclassOf<UCard> CardWidgetClass;

// private:
	
	ECardSelectPurpose CardSelectPurpose; // 카드 선택 목적 저장

	UFUNCTION()
	void HandleCardClicked(UCard* ClickedCard); // 카드 클릭 이벤트 핸들러 함수
};
