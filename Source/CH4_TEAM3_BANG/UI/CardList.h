// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Card/BangCardManager.h"
#include "CardList.generated.h"

class UButton;
class UCard;
class UHorizontalBox;
class UScrollBox;
class USizeBox;
class UCanvasPanel;
/**
 * 
 */

// 카드 선택 이벤트를 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardSelectedDelegate, FSingleCard, SelectedCard);
UCLASS()
class CH4_TEAM3_BANG_API UCardList : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CanvasPanel;

	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBox;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "CardList")
	UScrollBox* ScrollBox;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "CardList")
	UButton* UseCardButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "CardList")
	UCard* SelectedCardSlot;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "UI")
	TSubclassOf<UCard> CardWidgetClass;
public:
	// 선택된 카드 정보를 저장하는 변수
	UPROPERTY(BlueprintReadOnly, Category = "CardList")
	FSingleCard SelectedCard;
    
	// 선택된 카드 위젯 포인터
	UPROPERTY(BlueprintReadOnly, Category = "CardList")
	TObjectPtr<UCard> SelectedCardWidget;
    
	UPROPERTY(BlueprintAssignable, Category = "CardList")
	FOnCardSelectedDelegate OnCardSelected;
    
	// 카드를 선택하는 함수
	UFUNCTION(BlueprintCallable, Category = "CardList")
	void SelectCard(UCard* CardWidget);
	
	// UseCardButton 클릭 이벤트에 바인딩될 함수
	UFUNCTION()
	void OnUseCardButtonClicked();


	//컨트롤러에서 쓰는 함수
	// 카드를 추가하는 함수
	UFUNCTION(BlueprintCallable, Category = "CardList")
	UCard* AddCard(FSingleCard CardData);
	
	// 선택된 카드를 리스트에서 제거하는 함수
	UFUNCTION(BlueprintCallable, Category = "CardList")
	void RemoveSelectedCard();

	// 선택된 카드를 반환
	UFUNCTION(BlueprintCallable, Category = "CardList")
	FSingleCard GetSelectedCard() const { return SelectedCard; }

	
	
protected:
	virtual void NativeConstruct() override;
};
