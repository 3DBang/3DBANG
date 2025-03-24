// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CardDescriptionWidget.h"

#include "Components/TextBlock.h"

void UCardDescriptionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!DescriptionText)
	{
		UE_LOG(LogTemp, Error, TEXT("UMG Widget Binding Failed in UCardDescriptionWidget!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("UCardDescriptionWidget: DescriptionText Widget Binding Success!")); // 성공 로그 추가
	}
}

void UCardDescriptionWidget::SetDescriptionText(const FText& InTitleText, const FText& InDescriptionText)
{
	if (TitleText)
	{
		TitleText->SetText(InTitleText);	
	}
	
	if (DescriptionText)
	{
		DescriptionText->SetText(InDescriptionText);
	}
}