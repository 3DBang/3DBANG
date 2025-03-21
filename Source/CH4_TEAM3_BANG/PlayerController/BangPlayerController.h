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

public:
//서버에 턴 종료 요청 
	UFUNCTION(Server, Reliable)
	void Server_EndTurn(const uint32 UniqueID, ECharacterType PlayerCharacter);

	UFUNCTION(Server, Reliable)
	void Server_UseCard(EActiveType SelectedCard, uint32 TargetPlayerID);

///////////////////////////
////클라이언트 관련 로직 작성란
//////////////////////////
public:
	// 보유중인 카드 보기 (UI에서 클릭하면 카드 선택 가능)
	UFUNCTION(Client, Reliable)
	void Client_SelectCard();
	
	UFUNCTION(Client, Reliable)
	void Client_HandleCardSelection(EActiveType SelectedCard);

	UFUNCTION(Client,Reliable)
	void Client_SetControllerRotation(FRotator NewRotation);


	void Client_SetControllerRotation_Implementation(FRotator NewRotation);

	//void OnPossess(APawn* InPawn) override;

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

	UFUNCTION(Client, Reliable)
	void Client_SelectTarget();
};

