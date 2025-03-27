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
class UWidgetComponent;
class UTableCard;

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

	UPROPERTY()
	TObjectPtr<UBangCardManager> CardManager;

protected:
	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override; // PS가 다 생성되고 난 뒤에 호출
	
///////////////////////////
////서버 관련 로직 작성란
//////////////////////////
public:	
	UPROPERTY()
	bool bCanUseBang;

	UPROPERTY()
	TObjectPtr<class ABangPlayerState> BangMyPlayerState;

	UFUNCTION(Server, Reliable)
	void Server_UseCard(const FSingleCard& SingleCard, int32 TargetID);
	
	UFUNCTION(Server, Reliable)
	void Server_EndTurn();

	UFUNCTION(Server, Reliable)
	void Server_RequestPlayerListBroadcast();

///////////////////////////
////클라이언트 관련 로직 작성란
//////////////////////////
private:
	//사용중인 특정 카드
	FSingleCard UsingCard;
	//사용중인 카드 타입
	EActiveType UsingActiveType;
	//사용중인 카드들 초기화
	void InitializUsingCard();
	
public:
	UPROPERTY()
	uint32 PlayerUniqueID = 0;

	//PlayerUniqueID 설정
	UFUNCTION(BlueprintCallable)
	void InitPlayerUniqueID();
	
	UFUNCTION(BlueprintCallable)
	void JCH_Test();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	FString PlayerNickname;

	UFUNCTION(BlueprintImplementableEvent, Category = "Init")
	void Init();

	UFUNCTION(Client, Reliable)
	void Client_RequestCardSelection(int32 RequiredSelectCount,	ECardSelectPurpose Purpose);

	UFUNCTION()
	void OnCardSelectionComplete(
		FCardCollection SelectedCards,       // 플레이어가 실제로 선택한 카드들
		ECardSelectPurpose Purpose                      // 선택 목적
	);

	UFUNCTION(Client, Reliable)
	void Client_HandleCardSelection(const FSingleCard& SingleCard);

	UFUNCTION(Client,Reliable)
	void Client_SetControllerRotation(FRotator NewRotation);

	UFUNCTION()
	void HandleGeneralStoreSelectionComplete(const FSingleCard& SelectedCard);

	UFUNCTION(Client, Reliable)
	void Client_SelectTarget(const uint32 TargetPlayerID);

	//UFUNCTION(Client, Reliable)
	//void StealFromOpponent(const FSingleCard& SingleCard);

	// 호출 시점을 위젯에서 카드 보일때
	UFUNCTION()
	void UpdateCardList(FPlayerCollection& PlayerInfo);
	
	UFUNCTION(Client, Reliable)
	void Client_UpdateGameLogUI(const FString& GameLogMessage);

	UFUNCTION(Client, Reliable)
	void Client_UpdatePlayerListUI(const TArray<FPlayerInformation>& PlayerList);

	UFUNCTION(Server, Reliable)
	void Server_TestDrawCards();
	// 정빈
	UFUNCTION(Client, Reliable)
	void Client_ShowDrawnCards();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UTableCard> TableCard;
	
	UFUNCTION(Client, Reliable)
	void Client_OnTurnStart(const FCardCollection& DrawCards);
	
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

	// 마우스 클릭하면 호출 되는 함수

	UFUNCTION()
	void MouseClicked();
	
	FName TestPlayerController;

	UFUNCTION(Client, Reliable)
	void Client_OpenCamera(); // 여기에 추가적으로 PlayerStateID 들어가야함 

	UFUNCTION(Client, Reliable)
	void Client_SetInputEnabled(bool IsAttacker);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_OpenCamera();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_CloseCamera();

	UFUNCTION(Client, Reliable)
	void Client_CloseCamera();

	UFUNCTION(Client, Reliable)
	void Client_SetOutline(uint32 OtherPlayerUniqueID, bool bEnable, int32 StencilValue);

	UFUNCTION(Client, Reliable)
	void Client_ShowDrawCard(EShowTableCard ShowTableType);


	
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

	/**
	 * 서버에 게임 로그 메시지 전송을 요청하는 함수입니다.
	 * 이 함수는 Reliable 특성을 가지며 서버에서 호출됩니다.
	 *
	 * @param GameLogMessage 전송할 게임 로그 메시지
	 */
	UFUNCTION(Server, Reliable)
	void Server_RequestSendGameLog(const FString& GameLogMessage);

	UFUNCTION(Client, Reliable)
	void Client_ReceiveMessage(const FString& Message, const FString& FromNickname, const FString& ToPlayerNickname);

	// 플레이어에게 카드 선택권 요구
	UFUNCTION(Client, Reliable)
	void Client_RequestSelectCard(const uint32& FromUniqueID, const FPlayerCardCollection DrawCards);
	
	// 플레이어에게 카드 선택권 요구 응답
	UFUNCTION(Server, Reliable)
	void Server_RespondSelectCard();

	UFUNCTION()
	void PlayerInfoUpdatedEvent(FPlayerCollection FPlayerCollection);

	UFUNCTION()
	void TryBindPlayerInfoUpdated();

public:

	///////////////////////////
	//// 원명 추가 
	//////////////////////////
	UFUNCTION(Client, Reliable)
	void Client_ToggleMappingContext();

	//void SetWidgetVisibility(uint32 PlayerID, bool bVisible);
	//플레이어 스테이트에 유저에 대한 정보가 있는지 확인 
	void SetWidgetVisibility(uint32 PlayerID, bool bVisible);
	void GetUserInformationUI(uint32 BangPlayerStateID);

	//void GetPlayerStateAtBegin();

	void GetPlayerStateAtBeginTest(uint32 BangPlayerStateID);

	void RemoveBangPlayerState(uint32 BangPlayerStateID);

	UFUNCTION(Client, Reliable)
	void Client_GetPlayerStateAtBeginTest(uint32 BangPlayerStateID);

	UFUNCTION(Client, Reliable)
	void Client_RemoveBangPlayerState(uint32 BangPlayerStateID);
private:
	bool bIsCameraContextActive;

	TMap<uint32, UWidgetComponent*> PlayerWidgets;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* InteractionWidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> InteractionWidgetClass;

	uint32 ControllerPlayerStateID = INDEX_NONE;

public:
	void UpdatePlayerInfo(uint32 BangUniqueID,int32 HP, int32 Range);
	bool bIsFirstCameraMode = true;
	inline FString GetPlayerNickname(){return PlayerNickname;}

};



