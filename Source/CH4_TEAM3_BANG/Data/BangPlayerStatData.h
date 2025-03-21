#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BangPlayerStatData.generated.h"

UCLASS()
class CH4_TEAM3_BANG_API UBangPlayerStatData : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	FString PlayerId;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAlive;
};
