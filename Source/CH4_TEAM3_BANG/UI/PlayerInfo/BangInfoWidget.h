// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BangInfoWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class CH4_TEAM3_BANG_API UBangInfoWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
    virtual void NativeConstruct() override;

    /** Bound from UMG */
    UPROPERTY(meta = (BindWidget))
    UButton* CloseButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RangeText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* HealthText;
    //
private:
 
    UPROPERTY()
    int32 Range = 0;

    UPROPERTY()
    int32 CurrentHealth = 0;

public:
  
    UFUNCTION(BlueprintCallable)
    void UpdateRange(int32 NewRange);

    UFUNCTION(BlueprintCallable)
    void UpdateCurrentHealth(int32 NewHealth);

    UFUNCTION()
    void CloseWidget();
};
