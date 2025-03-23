#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BangGameInstance.generated.h"

class UBangCardManager;

USTRUCT(BlueprintType)
struct FCardManagerInstance
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UBangCardManager> CardManager;
};

UCLASS()
class CH4_TEAM3_BANG_API UBangGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	
	UFUNCTION()
	void GetCardManager(FCardManagerInstance& OutCardManager);

private:
	UPROPERTY()
	TObjectPtr<UBangCardManager> CardManager;
};
