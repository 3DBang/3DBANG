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
	// 카드 데이터 애셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card Manager")
	UBangCardDataAsset* CardData;

	// 모든 카드 섞기
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	void ShuffleDeck();

	// 모든 카드 가져오기
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	void GetAllCards();

	// 특정 이름의 카드을 찾는 함수 Sample
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	UBangCardBase* GetItemByName(FText Name) const;

	// 인원에 맞는 직업카드 추출 로직
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	TArray<UBangCardBase*> GetJobByPlayer(int PlayerCount);

private:
	// 모든 카드 저장 배열
	TArray<UBangCardBase*> AllCards;

	// 개별 카드 배열
	TArray<UBangCardBase*> CharacterCards;
	TArray<UBangCardBase*> PassiveCards;
	TArray<UBangCardBase*> ActiveCards;
	TArray<UBangCardBase*> JobCards;

	// 카드 타입별 저장
	TMap<ECardType, TArray<UBangCardBase*>> CardDeckByType;

	void ShuffleArray(TArray<UBangCardBase*>& Cards);
};
