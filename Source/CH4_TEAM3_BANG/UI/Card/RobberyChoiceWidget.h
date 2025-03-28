// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TableCard.h"
#include "RobberyChoiceWidget.generated.h"

/**
 * 
 */
UCLASS()
class CH4_TEAM3_BANG_API URobberyChoiceWidget : public UTableCard
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitializeRobberyCardList(
		const FCardCollection& FrontCards,
		const FCardCollection& BackCards,
		const ECardSelectPurpose CardSelectPurpose
	);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card")
	TObjectPtr<UTexture2D> BackCardIcon;
	
	UPROPERTY(meta = (BindWidget))
	UWrapBox* BackCardContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card")
	TSubclassOf<UCard> BackCardWidgetClass;
};
