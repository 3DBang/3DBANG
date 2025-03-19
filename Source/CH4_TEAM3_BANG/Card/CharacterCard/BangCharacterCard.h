#pragma once

#include "CoreMinimal.h"
#include "Card/BaseCard/BangCardBase.h"
#include "UObject/Object.h"
#include "BangCharacterCard.generated.h"

UCLASS(Blueprintable)
class CH4_TEAM3_BANG_API UBangCharacterCard : public UBangCardBase
{
	GENERATED_BODY()
	
public:
	UBangCharacterCard();

	// 케릭터카드 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card | Character")
	int Health;
};
