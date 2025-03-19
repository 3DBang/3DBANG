// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BangPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class CH4_TEAM3_BANG_API ABangPlayerController : public APlayerController
{
	GENERATED_BODY()
///////////////////////////
//// Enhanced Input
//////////////////////////


public:
	ABangPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> LookAction= nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> MenuAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> InformationAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> ZoomAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> ClickAction = nullptr;

	/**Test Sample*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> MoveAction = nullptr;

protected:
	virtual void BeginPlay() override;


///////////////////////////
////서버 관련 로직 작성란
//////////////////////////




///////////////////////////
////클라이언트 관련 로직 작성란
//////////////////////////


};
