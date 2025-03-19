#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BangCardDataAsset.generated.h"

class UBangCardBase;

UCLASS()
class CH4_TEAM3_BANG_API UBangCardDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Data")
	TArray<UBangCardBase*> Cards;
};
