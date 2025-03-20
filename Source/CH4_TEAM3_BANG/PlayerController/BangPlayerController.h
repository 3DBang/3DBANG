// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameMode/BangGameModeBase.h"
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
	TObjectPtr<UInputAction> LookAction = nullptr;

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
	
	UFUNCTION(Server, Reliable)
	void Server_UseCardReturn(bool IsAble);

protected:
	virtual void BeginPlay() override;


///////////////////////////
////서버 관련 로직 작성란
//////////////////////////
/*
	// 사용 버튼을 클릭하면, 선택된 카드를 GameMode에게 전달
	UFUNCTION(Server, Reliable, WithValidation)
	void UseCard(ECardType SelectedCard);

	// 공격 대상 선택 후, 대상과 사용한 카드 정보를 GameMode에게 전달
	UFUNCTION(Server, Reliable, WithValidation)
	void SelectTargetAndUseCard(APlayerState* TargetPlayer, ECardType UsedCard);
*/

///////////////////////////
////클라이언트 관련 로직 작성란
//////////////////////////
	
	// 보유중인 카드 보기 (UI에서 클릭하면 카드 선택 가능)
	//UFUNCTION(Client, Reliable)
	//void ShowOwnedCards();
	
public:
	UFUNCTION(Client,Reliable)
	void Client_SetControllerRotation(FRotator NewRotation);
};
