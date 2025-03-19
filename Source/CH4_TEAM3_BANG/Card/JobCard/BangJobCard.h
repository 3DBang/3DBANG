#pragma once

#include "CoreMinimal.h"
#include "Card/BaseCard/BangCardBase.h"
#include "UObject/Object.h"
#include "BangJobCard.generated.h"

UCLASS(Blueprintable)
class CH4_TEAM3_BANG_API UBangJobCard : public UBangCardBase
{
	GENERATED_BODY()

public:
	UBangJobCard();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card | Job")
	EJobType JobType;
};
