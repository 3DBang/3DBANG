#pragma once

#include "CoreMinimal.h"
#include "Card/BaseCard/BangCardBase.h"
#include "UObject/Object.h"
#include "BangActiveCard.generated.h"

UCLASS(Blueprintable)
class CH4_TEAM3_BANG_API UBangActiveCard : public UBangCardBase
{
	GENERATED_BODY()

public:
	UBangActiveCard();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card | Active")
	EActiveType ActiveType;
};
