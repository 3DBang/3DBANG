// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardDescriptionWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class CH4_TEAM3_BANG_API UCardDescriptionWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TitleText;		//카드 제목을 설명할 텍스트 블록
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DescriptionText; // 카드 설명을 표시할 텍스트 블록

	UFUNCTION(BlueprintCallable, Category = "CardDescription")
	void SetDescriptionText(const FText& InTitleText, const FText& Text); // 설명 텍스트 설정 함수

protected:
	virtual void NativeConstruct() override;
	
};
