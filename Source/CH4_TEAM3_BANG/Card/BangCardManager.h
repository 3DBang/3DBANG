#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BangCardManager.generated.h"

class UBangCardBase;
class UBangCardDataAsset;

UCLASS()
class CH4_TEAM3_BANG_API UBangCardManager : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card Manager")
	UBangCardDataAsset* CardData;

	// 특정 이름의 카드을 찾는 함수
	UFUNCTION(BlueprintCallable, Category = "Card Manager")
	UBangCardBase* GetItemByName(FText Name) const;
};
