#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Card/BangCardManager.h"
#include "Card.generated.h"

class UCanvasPanel;
class UCardDescriptionWidget;
class UImage;
class UBangCardBase;
class UBorder;
class USizeBox;

// 카드 클릭 이벤트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardClickedDelegate, UCard*, ClickedCard);

UCLASS()
class CH4_TEAM3_BANG_API UCard : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Card")
	USizeBox* SizeBox;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Card")
	UBorder* Border;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Card")
	UImage* CardHighlights;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Card")
	UImage* CardHoverHighlights;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Card") 
	UCanvasPanel* DescriptionCanvas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card")
	FSingleCard Card;
	
	UPROPERTY()
	TObjectPtr<UCardDescriptionWidget> DescriptionWidgetInstance;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Card")
	TSubclassOf<UCardDescriptionWidget> DescriptionWidgetClass;

	UPROPERTY()
	FTimerHandle DescriptionDelayTimerHandle;

	UFUNCTION()
	void ShowDescriptionWidgetDelayed();
	
	//카드가 클릭되면 알리는 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Card")
	FOnCardClickedDelegate OnCardClicked;

	//카드 초기화시 호출
	UFUNCTION(BlueprintCallable, Category = "Card")
	void InitializeWithCard(FSingleCard InCard);

	// 클릭 이벤트 처리 함수
	UFUNCTION()
	void OnBorderClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	
	// 카드 선택 상태 토글 함수
	UFUNCTION(BlueprintCallable, Category = "Card")
	void ToggleCardSelection();
	
	// 카드 선택 상태 설정 함수
	UFUNCTION(BlueprintCallable, Category = "Card")
	void SetCardSelected(bool bSelected);

	UFUNCTION(BlueprintCallable, Category = "Card")
	void UpdateCardHighlightVisibility();

	// 현재 카드의 선택 상태 반환
	UFUNCTION(BlueprintPure, Category = "Card")
	bool IsCardSelected() const { return bIsSelected; }

	UFUNCTION(BlueprintCallable, Category = "Card")
    void UpdateCardDisplay();

protected:
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override; // BeginDestroy 오버라이드 추가
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
private:
	// 카드가 현재 선택되었는지 여부
	UPROPERTY()
	bool bIsSelected;
	
};


