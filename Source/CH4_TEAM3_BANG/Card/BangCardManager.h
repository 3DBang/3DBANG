#pragma once

#include "CoreMinimal.h"
#include "Data/CardEnums.h"
#include "UObject/Object.h"
#include "BangCardManager.generated.h"

class UBangCardBase;
class UBangCardDataAsset;

UCLASS()
class CH4_TEAM3_BANG_API UBangCardManager : public UObject
{
	GENERATED_BODY()
	
public:
	// DataAssect 접근용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card Manager")
	UBangCardDataAsset* CardData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card Manager")
	TArray<UBangCardDataAsset*> AllCards;
	
	TMap<ECardType, TArray<UBangCardDataAsset*>> CardDeckByType;

	// 모든 카드 섞기
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	void ShuffleDeck();

	// 카드 타입별 정리
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	void CategorizeCardsByType();

	// 모든 카드 가져오기
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	void GetAllCards();

	// 특정 이름의 카드을 찾는 함수 Sample
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	UBangCardBase* GetItemByName(FText Name) const;
};
