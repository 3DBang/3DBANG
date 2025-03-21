// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterUIActor/BangUIActor.h"
#include "Components/TextRenderComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABangUIActor::ABangUIActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));
	SetRootComponent(TextRenderComponent);
	TextRenderComponent->SetOwnerNoSee(true);
	TextRenderComponent->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	TextRenderComponent->VerticalAlignment = EVerticalTextAligment::EVRTA_TextCenter;
}

void ABangUIActor::SetDisplayText(const FName& NewText)
{
	if (HasAuthority())
	{
		DisplayText = NewText;
		OnRep_DisplayText();
	}
}

void ABangUIActor::OnRep_DisplayText()
{
	if (TextRenderComponent)
	{
		FText Temp = FText::FromName(DisplayText);
		TextRenderComponent->SetText(Temp);
	}
}

void ABangUIActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABangUIActor, DisplayText);
}
