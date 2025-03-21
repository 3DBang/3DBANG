// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameMode/BangGameModeBase.h"
#include "BangPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ABangPlayerState;
class ABangCharacter;
enum class EJobType : uint8;
enum class ECharacterType : uint8;
class ABangGameMode;
class UCameraComponent;

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
	

protected:
	virtual void BeginPlay() override;
///////////////////////////
////서버 관련 로직 작성란
//////////////////////////
	
public:
//서버에 턴 종료 요청 
	/*UFUNCTION(Server, Reliable)
	void Server_EndTurn(const uint32 UniqueID, ECharacterType PlayerCharacter);

	UFUNCTION(Server, Reliable)
	void Server_UseCard(EActiveType SelectedCard, uint32 TargetPlayerID);*/

///////////////////////////
////클라이언트 관련 로직 작성란
//////////////////////////
public:
	// 보유중인 카드 보기 (UI에서 클릭하면 카드 선택 가능)
	UFUNCTION(Client, Reliable)
	void Client_SelectCard();
	void Client_SelectCard_Implementation();
	
	UFUNCTION(Client, Reliable)
	void Client_HandleCardSelection(EActiveType SelectedCard);
	void Client_HandleCardSelection_Implementation(EActiveType SelectedCard);
	UFUNCTION(Client,Reliable)
	void Client_SetControllerRotation(FRotator NewRotation);
	
	UFUNCTION(Client, Reliable)
	void Client_SelectTarget();
	void Client_SelectTarget_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_UseCard(EActiveType SelectedCard, uint32 TargetPlayerID);
	void Server_UseCard_Implementation(EActiveType SelectedCard, uint32 TargetPlayerID);

	void Client_SetControllerRotation_Implementation(FRotator NewRotation);

	UFUNCTION(Server, Reliable)
	void Server_UseCardReturn(bool IsAble);
	void Server_UseCardReturn_Implementation(bool IsAble);

	UFUNCTION(Server, Reliable)
	void Server_EndTurn(const uint32 UniqueID, ECharacterType PlayerCharacter);
	void Server_EndTurn_Implementation(const uint32 UniqueID, ECharacterType PlayerCharacter);
	void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdatePlayerUI(FName& NewText);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdatePlayerHP(int32 NewHP);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetInitializeHP(int32 NewHP);

///////////////////////////
//// 원명 추가 
//////////////////////////
public:
	virtual void Tick(float DeltaTime) override;
private:
	TObjectPtr<ABangCharacter> OtherPlayers;



	//id의 값을 PlayerState ->

public:
	void MouseClicked();

	/*UFUNCTION()
	void HandleInputClick();*/
public:
	FName TestPlayerController;

	UFUNCTION(Client, Reliable)
	void Client_OpenCamera(); // 여기에 추가적으로 PlayerStateID 들어가야함 

	void Client_OpenCamera_Implementation();

	UFUNCTION(Client, Reliable)
	void Client_SetInputEnabled(bool IsAttacker);

	void Client_SetInputEnabled_Implementation(bool IsAttacker);

	UFUNCTION(BlueprintCallable,Server, Reliable)
	void Server_OpenCamera();
	void Server_OpenCamera_Implementation();

	UCameraComponent* FindCameraByTag(APawn* Pawn, const FName& Tag);
};

