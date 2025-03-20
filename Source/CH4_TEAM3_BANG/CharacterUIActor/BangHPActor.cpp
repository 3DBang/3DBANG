// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterUIActor/BangHPActor.h"

// Sets default values
ABangHPActor::ABangHPActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABangHPActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABangHPActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

