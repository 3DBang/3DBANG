#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h" // 
#include "Card/BangCardManager.h"

#include "TableCard.generated.h"

UCLASS()
class CH4_TEAM3_BANG_API UTableCard : public UUserWidget //  반드시 UUserWidget 상속해야 함
{
	GENERATED_BODY()

public:
	void InitializeCardList(const TArray<FSingleCard>& Cards);

protected:
	UPROPERTY(meta = (BindWidget))
	class UCardList* CardListWidget;
};
