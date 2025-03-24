#pragma once

#include "CoreMinimal.h"
#include "Card/BangCardManager.h"
#include "Data/PlayerInformation.h"
#include "GameFramework/PlayerController.h"
#include "BangPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ABangPlayerState;
class ABangCharacter;
class ABangGameMode;
class UCameraComponent;

enum class EJobType : uint8;
enum class ECharacterType : uint8;

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
	UPROPERTY()
	TObjectPtr<class ABangPlayerState> BangMyPlayerState;

	UFUNCTION(Server, Reliable)
	void Server_UseCard(const FSingleCard& SingleCard, int32 TargetID);

	UFUNCTION(Server, Reliable)
	void Server_UseCardReturn(bool IsAble);


	UFUNCTION(Server, Reliable)
	void Server_EndTurn();

///////////////////////////
////클라이언트 관련 로직 작성란
//////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	FString PlayerNickname;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Init")
	void Init();
	
	// 보유중인 카드 보기 (UI에서 클릭하면 카드 선택 가능)
	UFUNCTION(Client, Reliable)
	void Client_SelectCard();
	
	UFUNCTION(Client, Reliable)
	void Client_HandleCardSelection(const FSingleCard& SingleCard);

	UFUNCTION(Client,Reliable)
	void Client_SetControllerRotation(FRotator NewRotation);
	
	UFUNCTION(Client, Reliable)
	void Client_SelectTarget();
	
	UFUNCTION(Client, Reliable)
	void Client_RequestDiscardCards(const FCardCollection& CurrentCards, int32 MaxAllowedCardCount);

///////////////////////////
//// 원명 추가 
//////////////////////////
	void UpdatePlayerUI(FName& NewText);
	void UpdatePlayerHP(int32 NewHP);
	void SetInitializeHP(int32 NewHP);

private:
	TObjectPtr<ABangCharacter> OtherPlayers;
	
	//id의 값을 PlayerState ->

public:
	void MouseClicked();
	FName TestPlayerController;

	UFUNCTION(Client, Reliable)
	void Client_OpenCamera(); // 여기에 추가적으로 PlayerStateID 들어가야함 

	UFUNCTION(Client, Reliable)
	void Client_SetInputEnabled(bool IsAttacker);

	UFUNCTION(BlueprintCallable,Server, Reliable)
	void Server_OpenCamera();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_CloseCamera();

	UFUNCTION(Client, Reliable)
	void Client_CloseCamera();

	UFUNCTION(Client, Reliable)
	void Client_SetOutline(bool bEnable, int32 StencilValue);

	UCameraComponent* FindCameraByTag(APawn* Pawn, const FName& Tag);
	
private:
	float CameraBlendElapsed = 0.f;
	FTimerHandle CameraBlendHandle;

	bool bIsCameraMode = false;
	double CameraOpenBlendStartTime = 0.f;
	FTimerHandle CameraOpenBlendTimerHandle;
	//혹시 동작 제대로 안할까봐 OpenCamera,CloseCamera용 타이머핸들 2개만들게요
	FTimerHandle CameraCloseBlendTimerHandle;

	// 지목 모드 타이머핸들
	FTimerHandle BangModeTimerHandle;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputMappingContext> CameraMappingContext = nullptr;
	
private:
	FTransform CachedBangCameraTransform;

	///////////////////////////
	//// 찬호 추가 
	//////////////////////////
public:
	// 현재 들고있는 카드 배열
	UPROPERTY()
	FCardCollection CurrentCardCollection;

	UPROPERTY() // 유저가 카드고를수있는 카드컬렉션,
	//선택 후 뽑은 카드는 배열에서 지우고 남은 카드는 Server_RespondSelectCard 호출해서 서버에 알려줘야함
	FCardCollection SelectCardCollection;
	
	UFUNCTION(Client, Reliable)
	void Client_DisplayBangUI();

	UFUNCTION(Server, Reliable)
	void Server_HUDLoaded();

	UFUNCTION()
	void NotifyHUDLoaded();

	UFUNCTION()
	void StartButtonCLicked();

	UFUNCTION(Server, Reliable)
	void Server_StartGame();

	UFUNCTION()
	void TestButtonCLicked();

	UFUNCTION(Server, Reliable)
	void Server_StartTest();
	
	UFUNCTION()
	void SendMessageToServer(FString Message);
	
	UFUNCTION(Server, Reliable)
	void Server_SendMessage(const FString& Message, const FString& FromNickname, const FString& ToPlayerNickname);
	
	UFUNCTION(Client, Reliable)
	void Client_ReceiveMessage(const FString& Message, const FString& FromNickname, const FString& ToPlayerNickname);
	
	// 플레이어에게 카드 선택권 요구
	UFUNCTION(Client, Reliable)
	void Client_RequestSelectCard(const uint32& PlayerUniqueID, const FPlayerCardCollection DrawCards);
	
	// 플레이어에게 카드 선택권 요구 응답
	UFUNCTION(Server, Reliable)
	void Server_RespondSelectCard();
};

