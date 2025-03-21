// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameMode/BangGameModeBase.h"
#include "BangPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ABangPlayerState;

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
	//서버에 공격 요청 
	//UFUNCTION(Server, Reliable)
	//void Server_AttackPlayer(ABangPlayerState* TargetPlayer);

	//서버에 턴 종료 요청 
	UFUNCTION(Server, Reliable)
	void Server_EndTurn();
///////////////////////////
////클라이언트 관련 로직 작성란
//////////////////////////
	//UFUNCTION(Client, Reliable)
	//void Client_SelectCard(const ECardType SelectedCard);

	//공격할 대상 선택 UI 표시 
	//UFUNCTION(Client, Reliable)
	//void Client_ShowAttackUI();

	//피해자가 회피할지 선택하도록 UI 표시
	//UFUNCTION(Client, Reliable)
	//void Client_AskDodge();

	// 게임 UI 업데이트 (턴, 체력, 카드 정보 등)
	//UFUNCTION(Client, Reliable)
	//void Client_UpdateGameUI(int32 CurrentTurnPlayerIndex, int32 PlayerHealth, int32 CardsInHand);
public:
	UFUNCTION(Client,Reliable)
	void Client_SetControllerRotation(FRotator NewRotation);

	void Client_SetControllerRotation_Implementation(FRotator NewRotation);

	//void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdatePlayerUI(FName& NewText);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdatePlayerHP(int32 NewHP);
	
};
