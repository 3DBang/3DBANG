// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Card/BangCardManager.h"
#include "CardList.generated.h"

class UTextBlock;
class UButton;
class UCard;
class UHorizontalBox;
class UScrollBox;
class USizeBox;
class UCanvasPanel;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUseCardDelegate, FCardCollection, SelectedCard, ECardSelectPurpose, Purpose);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTurnEndDelegate);
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

	//카드 사용버튼 텍스트 
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "CardList")
	UTextBlock* UseInputButtonText;
	
	// 카드 사용 버튼
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "CardList")
	UButton* UseInputButton;

	//카드 사용버튼 텍스트 
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "CardList")
	UTextBlock* HiddenCardListButtonText;
	
	// 카드 숨기기 버튼
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "CardList")
	UButton* HiddenCardListButton;

	//턴 종료 버튼
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "CardList")
	UButton* TurnEndButton;

	// 직업 카드 슬롯
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "CardList")
	UCard* JobCardSlot;

	// 캐릭터 카드 슬롯
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "CardList")
	UCard* CharacterCardSlot;

	// 카드 위젯 클래스
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "UI")
	TSubclassOf<UCard> CardWidgetClass;
	
public:
	// 선택된 카드 정보를 저장하는 변수
	UPROPERTY(BlueprintReadOnly, Category = "CardList")
	FCardCollection SelectedCardList;
    
	// 선택된 카드 위젯 포인터
	UPROPERTY(BlueprintReadOnly, Category = "CardList")
	TArray<TObjectPtr<UCard>> SelectedCardWidgetList;
	
	//카드가 선택되면 다른 카드들의 선택을 해제함
	UPROPERTY(BlueprintAssignable, Category = "CardList")
	FOnCardSelectedDelegate OnCardSelected;

	/**
	 * OnUseCard는 카드 사용이 발생했을 때 호출되는 델리게이트입니다.
	 * 선택된 카드 리스트와 현재 카드 선택 목적을 전달하여 해당 이벤트를 처리할 수 있습니다.
	 *
	 * 델리게이트 매개변수:
	 * - FCardCollection SelectedCardList: 선택된 카드들의 컬렉션을 나타냅니다.
	 * - ECardSelectPurpose CurrentCardSelectPurpose: 현재 카드 선택 목적을 나타냅니다.
	 *
	 * 이 델리게이트는 외부에서 바인딩하여 카드 사용 이벤트를 처리하는 데 사용할 수 있습니다.
	 */
	UPROPERTY(BlueprintAssignable, Category = "CardList")
	FOnUseCardDelegate OnUseCard;

	UPROPERTY(BlueprintAssignable, Category = "CardList")
	FOnTurnEndDelegate OnTurnEnd;
	
	// 카드 리스트 숨김 처리 불 변수
	UPROPERTY(BlueprintReadOnly, Category = "CardList")
	bool bIsHidden;

	// 카드 선택 목적
	UPROPERTY(BlueprintReadOnly, Category = "CardList")
	ECardSelectPurpose CurrentCardSelectPurpose;

	// 선택해야 할 카드 개수
	UPROPERTY(BlueprintReadOnly, Category = "CardList")
	int32 CardsToSelectCount;
	
	// 카드를 선택하는 함수
	UFUNCTION(BlueprintCallable, Category = "CardList")
	void SelectCard(UCard* CardWidget);
	
	// UseInputButton 클릭 이벤트에 바인딩될 함수
	UFUNCTION()
	void OnUseInputButtonClicked();

	// HiddenCardListButton 클릭 이벤트에 바인딩될 함수
	UFUNCTION()
	void OnHiddenCardListButtonClicked();

	// TurnEndButton 클릭 이벤트에 바인딩될 함수
	UFUNCTION()
	void OnTurnEndButtonClicked();

	//카드 리스트와 직업, 캐릭터 카드를 초기화 하는 함수
	UFUNCTION(BlueprintCallable, Category = "CardList")
	void ClearCards();
	
	//******* 컨트롤러에서 쓰는 함수 *********
	
	// 카드를 추가하는 함수
	UFUNCTION(BlueprintCallable, Category = "CardList")
	UCard* AddCard(FSingleCard CardData);

	// 캐릭터 카드추가
	void AddCardToCharacterCardSlot(FSingleCard CardData);
	
	// 직업카드 추가
	void AddCardToJobCardSlot(FSingleCard CardData);
	
	// 선택된 카드를 리스트에서 제거하는 함수
	UFUNCTION(BlueprintCallable, Category = "CardList")
	void RemoveSelectedCard(FSingleCard RemoveCard);
	
	
protected:
	virtual void NativeConstruct() override;
};
