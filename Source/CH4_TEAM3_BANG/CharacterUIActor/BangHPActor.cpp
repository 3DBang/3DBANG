// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterUIActor/BangHPActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABangHPActor::ABangHPActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true; 

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SceneRoot);
}
void ABangHPActor::SetHiddenActorState(bool bIsHiddenHp)
{
	if (HasAuthority())
	{
		bIsHpHidden = bIsHiddenHp;
		SetActorHiddenInGame(bIsHpHidden);
	}
}
void ABangHPActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABangHPActor::OnRep_HiddenState()
{
	SetActorHiddenInGame(bIsHpHidden);
}

void ABangHPActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABangHPActor, bIsHpHidden);
}

