// TableCard.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Card/BangCardManager.h"
#include "TableCard.generated.h"

class UWrapBox;
class UCard;

UCLASS()
class CH4_TEAM3_BANG_API UTableCard : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitializeCardList(const FCardCollection& Cards);

protected:
	UPROPERTY(meta = (BindWidget))
	UWrapBox* CardContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card")
	TSubclassOf<UCard> CardWidgetClass;
};
