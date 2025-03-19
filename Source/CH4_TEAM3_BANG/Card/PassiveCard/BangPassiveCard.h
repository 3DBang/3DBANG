#pragma once

#include "CoreMinimal.h"
#include "Card/BaseCard/BangCardBase.h"
#include "UObject/Object.h"
#include "BangPassiveCard.generated.h"

UCLASS()
class CH4_TEAM3_BANG_API UBangPassiveCard : public UBangCardBase
{
	GENERATED_BODY()

public:
	UBangPassiveCard();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card | Passive")
	EPassiveType PassiveType;
};
