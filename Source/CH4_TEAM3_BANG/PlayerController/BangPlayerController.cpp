#include "BangPlayerController.h"
#include "Card/ActiveCard/BangActiveCard.h"
#include "EnhancedInputSubsystems.h"
#include "Data/CardEnums.h"
#include "GameMode/BangGameMode.h"
#include "PlayerState/BangPlayerState.h"
#include "BangCharacter/BangCharacter.h"
#include "Card/BangCardManager.h"
#include "Card/BaseCard/BangCardBase.h"
#include "CharacterUIActor/BangUIActor.h"
#include "Camera/CameraComponent.h" 
#include "Camera/CameraActor.h"
#include "Materials/MaterialInterface.h"
#include "Camera/PlayerCameraManager.h"
#include "GameState/BangGameState.h"
#include "UI/BangPlayerHUD.h"
#include "Components/WidgetComponent.h"
#include "EngineUtils.h"
#include "Components/CapsuleComponent.h"
#include "UI/Card/CardList.h"
#include "UI/Chat/BangInGameChattingWidget.h"
#include "UI/Card/TableCard.h" 
#include "UI/Chat/PlayerListGameLog.h"
#include "UI/Card/CardDescriptionWidget.h"
#include "Data/PlayerInformation.h"
#include "UI/PlayerInfo/BangInfoWidget.h"
#include "Components/MeshComponent.h"
ABangPlayerController::ABangPlayerController()
{
}

void ABangPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				LocalPlayerSubsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}


	/*if (IsLocalController())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABangPlayerController::GetPlayerStateAtBegin);
	}*/

	// 호스트는 직접 동작 수행 해줘야함
	if (HasAuthority())
	{
		TryBindPlayerInfoUpdated();
	}
	/*FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	bShowMouseCursor = true;*/
	FTimerHandle InitDelayHandle;
	GetWorld()->GetTimerManager().SetTimer(InitDelayHandle, this, &ABangPlayerController::InitPlayerUniqueID, 3.0f, false);

}

void ABangPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	TryBindPlayerInfoUpdated();

	//GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABangPlayerController::GetPlayerStateAtBegin);
}

void ABangPlayerController::TryBindPlayerInfoUpdated()
{
	if (const TObjectPtr<ABangPlayerState> BangPlayerState = Cast<ABangPlayerState>(PlayerState))
	{
		if (!BangPlayerState->FOnPlayerInfoUpdated.IsAlreadyBound(this, &ABangPlayerController::PlayerInfoUpdatedEvent))
		{
			BangPlayerState->FOnPlayerInfoUpdated.AddDynamic(this, &ABangPlayerController::PlayerInfoUpdatedEvent);
		}
	}
}

void ABangPlayerController::InitPlayerUniqueID()
{
	if (ABangPlayerState* PS = GetPlayerState<ABangPlayerState>())
	{
		PlayerUniqueID = PS->PlayerUniqueID;
		UE_LOG(LogTemp, Log, TEXT("[Init] PlayerUniqueID 설정 완료: %d"), PlayerUniqueID);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Init] not PlayerState"));
	}
}

void ABangPlayerController::Client_SetControllerRotation_Implementation(FRotator NewRotation)
{
	if (IsLocalController())
	{
		SetControlRotation(NewRotation);
	}
}

void ABangPlayerController::Client_OnTurnStart_Implementation(const FCardCollection& DrawCards)
{
	bCanUseBang = true;
	UE_LOG(LogTemp, Warning, TEXT("[ABangPlayerController::Client_OnTurnStart_Implementation]: It's my turn! Controller Name: %s"), *GetName());
	ABangPlayerState* BangPlayerState= GetPlayerState<ABangPlayerState>();
	if (!BangPlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController::Client_OnTurnStart_Implementation]: PlayerState is null!"));
		return;
	}
	if (ABangPlayerHUD* BangHUD = Cast<ABangPlayerHUD>(GetHUD())) // HUD 캐스팅 및 유효성 검사
	{
		BangHUD->SetupTurnCardSelection();
		if (UCardList* CardListWidget = BangHUD->CardListWidgetInstance) // CardListWidgetInstance 유효성 검사
		{
			for (const FSingleCard& Card : DrawCards.CardList)
			{
				CardListWidget->AddCard(Card); // 카드 위젯 추가
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[ABangPlayerState::StartTurn] CardListWidgetInstance 없음 HUD 있음"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerState::StartTurn] BangHUD 없음"));
	}
	Client_RequestCardSelection(1, ECardSelectPurpose::UseCard);	
}

void ABangPlayerController::UpdateCardList(FPlayerCollection& PlayerInfo)
{
	UE_LOG(LogTemp, Log, TEXT("[ABangPlayerController::Client_UpdateCardList_Implementation] UI 카드리스트 업데이트"));

	ABangPlayerState* BangPlayerState = GetPlayerState<ABangPlayerState>();
	if (!BangPlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController::Client_UpdateCardList_Implementation] PlayerState 없음"));
		return;
	}

	//플레이어 스테이트의 플레이어 인포를 써보기
	FPlayerInformation* PlayerInformation =  BangPlayerState->PlayerInfo.GetPlayerInformation(PlayerUniqueID);
	FCardCollection MyCardCollection;
	BangPlayerState->GetCard(PlayerUniqueID, MyCardCollection); // PS에서 카드 정보 가져오기
	FSingleCard JobCard;
	FSingleCard CharacterCard;
	// 직업카드 및 캐릭터 카드 세팅
	if (PlayerInformation)
	{
		BangPlayerState->GetCardByJobType(PlayerInformation->JobCardType, JobCard);
		BangPlayerState->GetCardByCharacter(PlayerInformation->CharacterCardType, CharacterCard);
	}
	
	UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController::Client_UpdateCardList_Implementation] Player ID: %d"), PlayerUniqueID);

	if (ABangPlayerHUD* BangHUD = Cast<ABangPlayerHUD>(GetHUD())) // HUD 캐스팅 및 유효성 검사
	{
		if (UCardList* CardListWidget = BangHUD->CardListWidgetInstance) // CardListWidgetInstance 유효성 검사
		{
			CardListWidget->ClearCards(); // 기존 카드 리스트 비우기 여기서 캐릭터와 직업 카드도 지워줌

			CardListWidget->AddCardToJobCardSlot(JobCard);
			CardListWidget->AddCardToCharacterCardSlot(CharacterCard);
			
			for (const FSingleCard& Card : MyCardCollection.CardList)
			{
				CardListWidget->AddCard(Card); // 카드 위젯 추가
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController::Client_UpdateCardList_Implementation] CardListWidgetInstance 없음 HUD 있음"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController::Client_UpdateCardList_Implementation] BangHUD 없음"));
	}
}

void ABangPlayerController::UpdatePlayerUI(FName& NewText)
{
	if (HasAuthority())
	{
		ABangCharacter* BangCharacter = Cast<ABangCharacter>(GetPawn());
		if (BangCharacter && BangCharacter->TextActor.IsValid())
		{
			BangCharacter->TextActor->SetDisplayText(NewText);
		}
	}
}

void ABangPlayerController::UpdatePlayerHP(int32 NewHP)
{
	if (HasAuthority())
	{
		ABangCharacter* BangCharacters = Cast<ABangCharacter>(GetPawn());
		if (BangCharacters)
		{
			BangCharacters->UpdateHPActors(NewHP);
		}
	}

}

void ABangPlayerController::SetInitializeHP(int32 NewHP)
{
	if (HasAuthority())
	{
		ABangCharacter* BangCharacterHP = Cast<ABangCharacter>(GetPawn());
		if (BangCharacterHP)
		{
			BangCharacterHP->SetHP(NewHP);
		}
	}
}


/**
 * 카드 선택에 대한 클라이언트 작업을 처리합니다. 선택된 카드와 필요에 따라 타겟 플레이어를 기반으로 적절한 서버 작업을 실행합니다.
 *
 * @param SingleCard 선택된 카드에 대한 정보를 포함하는 구조체입니다.
 */
void ABangPlayerController::Client_HandleCardSelection_Implementation(const FSingleCard& SingleCard)
{
	ABangPlayerHUD* BangHUD = Cast<ABangPlayerHUD>(GetHUD());
	UCardList* CardList = BangHUD->CardListWidgetInstance;

    uint32 TargetPlayerID = 0; // 기본값, 상대가 필요하면 SelectTarget()에서 설정
	if (!SingleCard.Card)return;
	UE_LOG(LogTemp, Warning, TEXT("HandleCardSelection"));
	EActiveType OutActiveType;
	EPassiveType OutPassiveType;

	ABangPlayerState* PS = GetPlayerState<ABangPlayerState>();
	if (!PS) return;

	FPlayerInformation* Info = PS->PlayerInfo.GetPlayerInformation(PlayerUniqueID);
	if (!Info)return;

	PS->GetCardType(PlayerUniqueID, SingleCard, OutActiveType, OutPassiveType);
	UE_LOG(LogTemp, Warning, TEXT("OutActiveType: %s, OutPassiveType: %s"),
		*UEnum::GetValueAsString(OutActiveType),
		*UEnum::GetValueAsString(OutPassiveType));

	if (OutActiveType == EActiveType::Missed)return;

	bool bNeedsTarget = (OutActiveType == EActiveType::Bang ||
		OutActiveType == EActiveType::Robbery ||
		OutActiveType == EActiveType::CatBalou ||
		OutActiveType == EActiveType::Duel ||
		OutActiveType == EActiveType::Jail);

	if (bNeedsTarget)
	{
		//사용할 카드와 카드 타입 저장
		UsingCard = SingleCard;
		UsingActiveType = OutActiveType;
		Server_OpenCamera_Implementation();
		//적 선택단계로 넘어가기(탑뷰)
	}
	else
	{
		//사용후 초기화
		//Server_UseCard(SingleCard, TargetPlayerID);
		CardList->RemoveSelectedCard(SingleCard);
		InitializUsingCard();
	}
}


void ABangPlayerController::Server_EndTurn_Implementation()
{
	ABangPlayerState* PS = GetPlayerState<ABangPlayerState>();
	if (!PS)return;

	FPlayerInformation* MyInfo = PS->PlayerInfo.GetPlayerInformation(PlayerUniqueID);
	if (!MyInfo)
	{
		UE_LOG(LogTemp, Error, TEXT("[Controller] PlayerInfo not found for ID: %d"), PlayerUniqueID);
		return;
	}

	const int32 CurrentHealth = MyInfo->CurrentHealth;
	const int32 CardCount = MyInfo->MyCards.PlayerCards.Num();

	if (CardCount > CurrentHealth)
	{
		// 클라이언트에 카드 버리기 UI 요청
		Client_RequestCardSelection(CardCount-CurrentHealth, ECardSelectPurpose::DiscardCard);
		return;
	}
	else
	{
		PS->Server_EndTurn(PlayerUniqueID);
		Client_RequestCardSelection(1, ECardSelectPurpose::None);
	}
}

void ABangPlayerController::JCH_Test()
{
	ABangPlayerState* PS = GetPlayerState<ABangPlayerState>();
	if (PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerUniqueID : %d,"), PS->PlayerUniqueID);
	}
	ABangPlayerHUD* BangPlayerHUD = Cast<ABangPlayerHUD>(GetHUD());
	BangPlayerHUD->CardListWidgetInstance->OnUseCard.AddDynamic(this, &ABangPlayerController::OnCardSelectionComplete);
	Client_RequestCardSelection(1, ECardSelectPurpose::RespondToDuel);
}

//유저 입력 상황 UI연동
void ABangPlayerController::Client_RequestCardSelection_Implementation(
	int32 RequiredSelectCount,
	ECardSelectPurpose Purpose)
{
	FText ButtonText = FText::FromString(TEXT("선택"));

	UE_LOG(LogTemp, Warning, TEXT("[TEST] Client_RequestCardSelection_Implementation called"));
	switch (Purpose)
	{
	case ECardSelectPurpose::UseCard:
	{
		ButtonText = FText::AsCultureInvariant(L"사용하기");
		break;
	}
	case ECardSelectPurpose::DiscardCard:
	{
		ButtonText = FText::AsCultureInvariant(L"버리기");
		break;
	}
	case ECardSelectPurpose::GeneralStoreDraft:
		// 잡화점 – 전체 플레이어가 순서대로 카드 중 1장 선택
			// 남은 카드가 없다면 잡화점 종료 처리
		break;

	case ECardSelectPurpose::KitCarlsonDrawCard:
		// 키트 칼슨 능력 – 카드 3장 중 2장 선택
		//MyInfo.SelectableCards 를 화면에 띄워야하고 뽑아야되는 카드 수는 2
		break;

	case ECardSelectPurpose::StealFromOpponent:
		// 상대의 보유 카드 중 1장을 선택 (정보가 안 보일 수 있음)
		//MyInfo.SelectableCards = Target.MyCard 를 화면에 뒷면으로 띄워야하고 뽑아야되는 카드 수는 1
		break;

	case ECardSelectPurpose::RespondToDuel:
	{
		ButtonText = FText::AsCultureInvariant(L"응수하기");
		break;
	}
	case ECardSelectPurpose::RespondToIndians:
	{
		ButtonText = FText::AsCultureInvariant(L"쫓아내기");
		break;
	}
	case ECardSelectPurpose::RespondToAttack:
	{
		ABangPlayerState* PS = GetPlayerState<ABangPlayerState>();
		if (PS->CheckIsCardAbleByPassive(PlayerUniqueID, EPassiveType::Barrel))
		{
			PS->Server_CheckCardSymbol(PlayerUniqueID, 1);
			return;
		}
		ButtonText = FText::AsCultureInvariant(L"회피하기");
		break;
	}
	default:
		ButtonText = FText::AsCultureInvariant(TEXT("선택"));
		break;
	}
	bool bMyCardCollection =
			(Purpose == ECardSelectPurpose::UseCard) ||
			(Purpose == ECardSelectPurpose::DiscardCard) ||
			(Purpose == ECardSelectPurpose::RespondToDuel) ||
			(Purpose == ECardSelectPurpose::RespondToIndians) ||
			(Purpose == ECardSelectPurpose::RespondToAttack);

	if (bMyCardCollection)
	{
		if (ABangPlayerHUD* BangHUD = Cast<ABangPlayerHUD>(GetHUD()))
		{
			BangHUD->SetupTurnCardSelection(Purpose, ButtonText, RequiredSelectCount);
		}
	}
}
void ABangPlayerController::InitializUsingCard()
{
	//UsingCard = nullptr;
	UsingActiveType = EActiveType::None;
}


//카드 선택 시 호출 함수
void ABangPlayerController::OnCardSelectionComplete(
	FCardCollection SelectedCards,       // 플레이어가 실제로 선택한 카드들  
	ECardSelectPurpose Purpose)          // 선택 목적
{
	UE_LOG(LogTemp, Warning, TEXT("SelectedCards.Num(): %d"), SelectedCards.CardList.Num());
	UE_LOG(LogTemp, Warning, TEXT("Purpose: %s"), *UEnum::GetValueAsString(Purpose));

	ABangPlayerState* PS = GetPlayerState<ABangPlayerState>();
	if (!PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("NoPS"));
		return;
	}
	FPlayerInformation* MyInfo = PS->PlayerInfo.GetPlayerInformation(PlayerUniqueID);
	if (!MyInfo)
	{
		UE_LOG(LogTemp, Warning, TEXT("NoInfo"));
		return;
	}

	EActiveType OutActiveType;
	EPassiveType OutPassiveType;


	switch (Purpose)
	{
	case ECardSelectPurpose::UseCard:
	{
		//카드 사용하기
		UE_LOG(LogTemp, Warning, TEXT("[TEST]OnCardSelectionComplete UseCard"));
		Client_HandleCardSelection(SelectedCards.CardList[0]);
		break;
	}
	case ECardSelectPurpose::DiscardCard:
	{
		for (const FSingleCard& Card : SelectedCards.CardList)
		{
			//보유 카드에서 제거 후 버린카드덱에 추가
			MyInfo->MyCards.RemoveCard(Card.Card->SymbolType, Card.Card->SymbolNumber);
			PS->RestoreCard(PlayerUniqueID, Card);
			PS->Server_SetPlayerInfo(PS->PlayerInfo);
		}
		Server_EndTurn();
		break;
	}

	case ECardSelectPurpose::GeneralStoreDraft:
	{
		MyInfo->MyCards.AddCardCollectionToPlayerCards(SelectedCards);
		HandleGeneralStoreSelectionComplete(SelectedCards.CardList[0]);
		// 잡화점 – 전체 플레이어가 순서대로 카드 중 1장 선택
		break;
	}
	case ECardSelectPurpose::KitCarlsonDrawCard:
	{
		// 키트 칼슨 능력 – 카드 3장 중 2장 선택
		// 안뽑은 카드 걸러내기
		TArray<FSingleCard> NotChosenCards;
		/*for (const FSingleCard& Card : CardsToChooseFrom.CardList)
		{
			if (!SelectedCards.Contains(Card))
			{
				NotChosenCards.Add(Card);
			}
		}*/
		/*
		FCardCollection SelectedCardCollection;
		SelectedCardCollection.CardList = SelectedCards.CardList;
		MyInfo->MyCards.AddCardCollectionToPlayerCards(SelectedCardCollection);

		// 선택되지 않은 카드들 제거를 위해 변환
		FPlayerCardCollection RefundCards;
		for (const FSingleCard& Card : NotChosenCards)
		{
			FPlayerCardSymbol Symbol;
			Symbol.SymbolNumber = Card.Card->SymbolNumber;
			Symbol.SymbolType = Card.Card->SymbolType;
			RefundCards.PlayerCards.Add(Symbol);
		}

		// GameMode로 카드 돌려보내기
		if (ABangGameMode* GM = GetWorld()->GetAuthGameMode<ABangGameMode>())
		{
			GM->RefundCards(RefundCards);
		}

		// 선택되지 않은 카드들을 MyInfo에서도 제거 (혹시 들어가있을 수 있으니)
		for (const FPlayerCardSymbol& RefundCard : RefundCards.PlayerCards)
		{
			MyInfo->MyCards.RemoveCard(RefundCard.SymbolType, RefundCard.SymbolNumber);
		}*/
		break;
	}

	case ECardSelectPurpose::StealFromOpponent:
	{	
		// 상대의 보유 카드 중 1장을 선택 
		// 상대 카드 중 1장 없애기
		// 내 카드 덱에 1장 추가하기
		break;
	}
	case ECardSelectPurpose::RespondToDuel:
	{	// 결투 중 뱅 카드 선택
		if (SelectedCards.CardList.Num() == 0)
		{
			//PS->LoosePlayerHealth(PlayerUniqueID, 1);
		}
		PS->GetCardType(PlayerUniqueID, SelectedCards.CardList[0], OutActiveType, OutPassiveType);
		if(OutActiveType == EActiveType::Bang)
		{
			// 뱅 카드 사용(결투 반격 성공)
			// 카드 지우기
			PS->RestoreCard(PlayerUniqueID, SelectedCards.CardList[0]);
		}
		else
		{
			Client_RequestCardSelection_Implementation(1, ECardSelectPurpose::RespondToDuel);
			//잘못된 카드 사용 처리
		}
		break;
	}
	case ECardSelectPurpose::RespondToIndians:
	{// 인디언 카드 대응 – 뱅 카드 선택
		if (SelectedCards.CardList.Num() == 0)
		{
			//PS->LoosePlayerHealth(PlayerUniqueID, 1);
		}
		PS->GetCardType(PlayerUniqueID, SelectedCards.CardList[0], OutActiveType, OutPassiveType);
		if (OutActiveType == EActiveType::Bang)
		{
			// 뱅 카드 사용(인디언 쫓아내기 성공)
			PS->RestoreCard(PlayerUniqueID, SelectedCards.CardList[0]);
		}
		else
		{
			//잘못된 카드 사용 처리
			Client_RequestCardSelection_Implementation(1, ECardSelectPurpose::RespondToIndians);
			UE_LOG(LogTemp, Error, TEXT("NonoBang!"));
			return;
		}
		break;
	}

	case ECardSelectPurpose::RespondToAttack:
	{	// Bang, Gatling 등의 공격에 대해 Missed 카드 선택
		if (SelectedCards.CardList.Num() == 0)
		{
			//PS->LoosePlayerHealth(PlayerUniqueID, 1);
		}
		PS->GetCardType(PlayerUniqueID, SelectedCards.CardList[0], OutActiveType, OutPassiveType);
		if (OutActiveType == EActiveType::Missed)
		{
			// 회피 카드 사용(회피 성공)
			PS->RestoreCard(PlayerUniqueID, SelectedCards.CardList[0]);
			MyInfo->MyCards.RemoveCard(SelectedCards.CardList[0].Card->SymbolType, SelectedCards.CardList[0].Card->SymbolNumber);
			PS->Server_SetPlayerInfo(PS->PlayerInfo);
		}
		else
		{
			//잘못된 카드 사용 처리
			UE_LOG(LogTemp, Error, TEXT("Missed!"));
			Client_RequestCardSelection_Implementation(1, ECardSelectPurpose::RespondToAttack);
			return;
		}
		break;
	}

	default:
		break;
	}
}

///////////////////////////
//// 찬호 추가 
//////////////////////////

void ABangPlayerController::Client_DisplayBangUI_Implementation()
{
	if (const TObjectPtr<ABangPlayerHUD> BangHUD = Cast<ABangPlayerHUD>(GetHUD()))
	{
		BangHUD->ChattingWidgetInstance->AddMessage(
			FText::FromString(FString::Printf(TEXT("Hello from %d"), GetUniqueID())),
			FSlateColor(FLinearColor::Green)
		);
	}	
}


void ABangPlayerController::NotifyHUDLoaded()
{
	Server_HUDLoaded();
	if (!HasAuthority())
	{
		if (const TObjectPtr<ABangPlayerHUD> BangHUD = Cast<ABangPlayerHUD>(GetHUD()))
		{
			BangHUD->ChattingWidgetInstance->StartButton->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABangPlayerController::Server_HUDLoaded_Implementation()
{
	const TObjectPtr<ABangGameMode> GameMode = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController] BeginPlay Controller GameMode is NULL!"));
		return;
	}

	GameMode->UpdatePlayerHUD();
}

/**
 * 서버로 채팅 메시지를 전송합니다. 입력된 메시지가 귓속말일 경우 타겟 플레이어 닉네임을 추출하고,
 * 일반 메시지일 경우 전체 채팅으로 전달됩니다.
 *
 * @param Message 전송하려는 채팅 메시지의 문자열입니다. 귓속말의 경우 "/{타겟 플레이어 아이디} {메시지}" 형식으로 전달됩니다.
 */
void ABangPlayerController::SendMessageToServer(FString Message)
{
	if (Message.IsEmpty()) return;

	FString ToPlayerNickname = "";

	// 귓속말의 경우 /{플레이어 아이디} {채팅내용}
	if (Message[0] == '/')
	{
		// 귓속말
		FString RawContent = Message.RightChop(1);

		FString TargetIDString;
		FString ChatContent;

		if (RawContent.Split(TEXT(" "), &TargetIDString, &ChatContent))
		{
			ToPlayerNickname = TargetIDString;
		}
	}

	// 전체챗팅
	Server_SendMessage(Message, PlayerNickname, ToPlayerNickname);
}

// PlayerState에서 값이 갱신되면 호출
void ABangPlayerController::PlayerInfoUpdatedEvent(FPlayerCollection FPlayerCollection)
{
	for (FPlayerInformation PlayerInfo : FPlayerCollection.Players)
	{
		UE_LOG(LogTemp, Display, TEXT("[PlayerInfoUpdatedEvent]"));
		UE_LOG(LogTemp, Display, TEXT("[PlayerInfoUpdatedEvent] %d"), PlayerInfo.PlayerUniqueID);
		UE_LOG(LogTemp, Display, TEXT("[PlayerInfoUpdatedEvent] %s"), *PlayerInfo.PlayerName);
	}
	for (FPlayerInformation PlayerInfo : FPlayerCollection.Players)
	{
		GetPlayerStateAtBeginTest(PlayerInfo.PlayerUniqueID);
		UpdatePlayerInfo(PlayerInfo.PlayerUniqueID,
			PlayerInfo.CurrentHealth,
			PlayerInfo.CharacterRange
		);
		//제거할때 제거하는것도 해야함 
	}
	// 플레이어 인포가 바겼을떄 변경돼야 하는것들
	// 카드정보, 플레이어 정보
	// 선택시에 카드정보 동기화

	// HUD에 접근해서 Map 데이터 갱신 + 플레이어 수도 맞춰야겠죠

	// 상대 스테이터스 info 갱신 호스트에서 호출 안됨;;;;;;;;;;;;;;;;;;;;;
	UpdateCardList(FPlayerCollection);
}

// 플레이어에게 카드 선택권 요구 응답
void ABangPlayerController::Server_RespondSelectCard_Implementation()
{
	FPlayerCardCollection PlayerCardCollection;
	for (auto [Card] : SelectCardCollection.CardList)
	{
		FPlayerCardSymbol SingleCard;
		SingleCard.SymbolNumber = Card->SymbolNumber;
		SingleCard.SymbolType = Card->SymbolType;
		PlayerCardCollection.PlayerCards.Add(SingleCard);
	}
	
	SelectCardCollection.CardList.Empty();
	
	const TObjectPtr<ABangGameMode> GameMode = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController] BeginPlay Controller GameMode is NULL!"));
		return;
	}
	
	GameMode->RefundCards(PlayerCardCollection);
}

// 플레이어에게 카드 선택권 요구
void ABangPlayerController::Client_RequestSelectCard_Implementation(const uint32& FromUniqueID, const FPlayerCardCollection DrawCards)
{
	if (DrawCards.PlayerCards.Num() == 0) return;

	if (IsLocalController() && GetUniqueID() == FromUniqueID)
	{
		ABangPlayerState* BangPlayerState = GetPlayerState<ABangPlayerState>();
		BangPlayerState->GetCard(FromUniqueID, SelectCardCollection);
		
		// 플레이어에게 카드 선택권 요구
		
		
		// 선택한 카드 배열에서 지우기
		// SelectCardCollection
	}
}

void ABangPlayerController::Client_ReceiveMessage_Implementation(const FString& Message, const FString& FromNickname, const FString& ToPlayerNickname)
{
	if (Message.IsEmpty()) return;

	if (FromNickname.IsEmpty())
	{
		// 전역
		if (const TObjectPtr<ABangPlayerHUD> BangHUD = Cast<ABangPlayerHUD>(GetHUD()))
		{
			BangHUD->ChattingWidgetInstance->AddMessage(
				FText::FromString(FString::Printf(TEXT("%s: %s"),*FromNickname, *Message)),
				FSlateColor(FLinearColor::White)
			);
		}	
	}
	else
	{
		// 특정
		if (PlayerNickname == FromNickname)
		{
			if (const TObjectPtr<ABangPlayerHUD> BangHUD = Cast<ABangPlayerHUD>(GetHUD()))
			{
				BangHUD->ChattingWidgetInstance->AddMessage(
					FText::FromString(FString::Printf(TEXT("%s: %s"),*FromNickname, *Message)),
					FSlateColor(FLinearColor::Red)
				);
			}
		}
	}
}

/**
 * 클라이언트로부터 메시지를 수신하고, 해당 메시지를 게임 상태를 통해 브로드캐스트합니다.
 *
 * @param Message 클라이언트가 전송한 메시지입니다.
 * @param FromNickname 메시지를 보낸 플레이어의 닉네임입니다.
 * @param ToPlayerNickname 메시지를 받을 플레이어의 닉네임입니다.
 */
void ABangPlayerController::Server_SendMessage_Implementation(const FString& Message, const FString& FromNickname, const FString& ToPlayerNickname)
{
	if (ABangGameState* BangGameState = GetWorld()->GetGameState<ABangGameState>())
	{
		BangGameState->BroadcastChatMessage(Message, FromNickname, ToPlayerNickname);
	}
}

void ABangPlayerController::Server_StartGame_Implementation()
{
	const TObjectPtr<ABangGameMode> GameMode = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController] BeginPlay Controller GameMode is NULL!"));
		return;
	}

	GameMode->ForceUpdate_StartGame_Real();
}

void ABangPlayerController::StartButtonCLicked()
{
	//JCH_Test();
	//Server_StartGame();
}

void ABangPlayerController::Server_StartTest_Implementation()
{
	const TObjectPtr<ABangGameMode> GameMode = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController] BeginPlay Controller GameMode is NULL!"));
		return;
	}

	GameMode->StartTest();
}

/**
 * 테스트 버튼 클릭 시 호출되는 함수입니다.
 * 서버 측에서 테스트를 위한 다양한 작업을 트리거합니다.
 *
 * 내부적으로 다음 동작을 수행합니다:
 * 1. 서버 테스트 시작 (Server_StartTest)
 * 2. 플레이어 리스트 브로드캐스트 요청 (Server_RequestPlayerListBroadcast)
 * 3. 카드 드로우 테스트 (Server_TestDrawCards)
 * 4. 게임 로그 송신 요청 (Server_RequestSendGameLog)
 */
void ABangPlayerController::TestButtonCLicked()
{
	UE_LOG(LogTemp, Error, TEXT("TestButtonCLicked"));
	// 로직 작성
	
	Server_StartTest();
	Server_RequestPlayerListBroadcast();
	Server_TestDrawCards();

	// 테스트 버튼을 누르면 로그가 찍히는듯? 여기서 왜 로그를 찍는지를 변수로 보내줘야 할듯?
	Server_RequestSendGameLog(FString::Printf(TEXT("게임 시작")));
	// 플레이어 스테이트에서
	// 카드 사용
	// 턴 오는거
	// 누가 죽고
	// 피까이고
	
}

///////////////////////////
//// 원명 추가 
//////////////////////////
void ABangPlayerController::MouseClicked()
{
	if (!IsLocalController())
	{
		return;
	}
	ABangPlayerState* ThisBangState = Cast<ABangPlayerState>(PlayerState);
	if (!ThisBangState)
	{
		return;
	}
	
	auto InformationThis = ThisBangState->PlayerInfo.GetPlayerInformation(ThisBangState->PlayerUniqueID);
	if (!InformationThis)
	{
		return;
	}

	//문제점1. 이러면 유저가 자기턴이 아닐경우에는 다른 유저를 클릭해 정보를 볼 수 없다.
	//정보는 탑뷰에서만 막는다 

	/*if (!Information->bIsMyTurn) 
	{
		return;
	}*/


	

	
	FHitResult HitResult;
	if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, HitResult))
	{
		DrawDebugSphere(GetWorld(), HitResult.Location, 10.f, 8, FColor::Red, false, 1.5f);
		ACharacter* HitChar = Cast<ACharacter>(HitResult.GetActor());
		if (HitChar && HitChar != GetPawn())
		{
			if (ABangCharacter* OtherPlayer = Cast<ABangCharacter>(HitChar))
			{
				ABangPlayerState* BangState = Cast<ABangPlayerState>(OtherPlayer->GetPlayerState());
				if (!BangState)
				{
					return;
				}
				ABangPlayerHUD* BangPlayerHUD = Cast<ABangPlayerHUD>(GetHUD());
				
				BangPlayerHUD->ShowBangInfoWidget(BangState->PlayerUniqueID, bIsCameraMode);
			}
		}
	}
	CurrentMouseCursor = EMouseCursor::Default;
}

void ABangPlayerController::Client_OpenCamera_Implementation()
{
	if (!IsLocalController())
	{
		return;
	}

	AGameStateBase* BGameState = GetWorld()->GetGameState<AGameStateBase>();
	if (!BGameState)
	{
		return;
	}
	int32 PlayerCount = BGameState->PlayerArray.Num();
	


	if (ABangCharacter* BangPlayer = Cast<ABangCharacter>(GetPawn()))
	{
		if (BangPlayer->GetFirstPersonMode())
		{
			BangPlayer->GetMesh()->SetVisibility(true);
		}

		UCameraComponent* StartCam = BangPlayer->FollowCamera;
		UCameraComponent* EndCam = BangPlayer->BangCamera;
		if (!StartCam || !EndCam) return;


		const FTransform StartTransform = StartCam->GetComponentTransform();
		ACameraActor* TempCam = GetWorld()->SpawnActor<ACameraActor>(
			ACameraActor::StaticClass(), StartTransform);
		if (!TempCam) return;


		BangPlayer->FollowCamera->Deactivate();
		BangPlayer->BangCamera->Deactivate();

		constexpr float BlendTime = 5.f;
		CameraOpenBlendStartTime = FPlatformTime::Seconds();
		SetViewTarget(TempCam);

		const FVector StartLocation = StartTransform.GetLocation();
		const FVector EndLocation = EndCam->GetComponentLocation() + 200.f; // 마지막에 회전하는 효과를 주고 싶어서 벡터를 사용해서 300f만큼 이동 그러면 마지막에 꿀벌마냥 회전할것

		//BangCamera의 위치를 한번 봐야할듯
		const FVector FlagLocation = BangPlayer->GetFlagLocation();

		GetWorldTimerManager().SetTimer(CameraOpenBlendTimerHandle, FTimerDelegate::CreateLambda(
			[this, BangPlayer, TempCam, StartLocation, EndLocation, FlagLocation, PlayerCount]() mutable
			{
				//좋아..상대시간 굳 
				float Elapsed = FPlatformTime::Seconds() - CameraOpenBlendStartTime;
				float Alpha = FMath::Clamp(Elapsed / BlendTime, 0.f, 1.f);

				FVector NewLoc = FMath::Lerp(StartLocation, EndLocation, Alpha);
				TempCam->SetActorLocation(NewLoc);
				TempCam->SetActorRotation((FlagLocation - NewLoc).Rotation());
				if (Alpha >= 1.f)
				{
					GEngine->AddOnScreenDebugMessage(
						-1,
						5.f,
						FColor::Red,
						TEXT("Alpha")
					);
					//Re -> PlayerUniqueID -> FindIndex 
					float Radian = (2 * PI / PlayerCount) * (PlayerUniqueID - 1);
					float Degree = FMath::RadiansToDegrees(Radian);
					FRotator CurrentRotation = BangPlayer->BangCamera->GetComponentRotation();
					CurrentRotation.Yaw += Degree;
					BangPlayer->BangCamera->SetRelativeRotation(CurrentRotation);

					BangPlayer->BangCamera->Activate();
					SetViewTarget(BangPlayer);
					bIsCameraMode = true;
					GetWorldTimerManager().ClearTimer(CameraOpenBlendTimerHandle);
					if (GetWorldTimerManager().IsTimerActive(CameraOpenBlendTimerHandle))
					{
						GEngine->AddOnScreenDebugMessage(
							-1,
							5.f,
							FColor::Red,
							TEXT("Error OpenTimer Acive")
						);
						TempCam->Destroy();
					}
					TempCam->Destroy();
				}
			}), 0.01f, true);
		GetWorldTimerManager().SetTimer(BangModeTimerHandle, this, &ABangPlayerController::Server_CloseCamera, 300.f, false);
	}

}

void ABangPlayerController::Client_SetInputEnabled_Implementation(bool IsAttacker)
{
	if (!IsLocalController())
	{
		return;
	}

	if (IsAttacker)
	{
		if (auto LocalPlayer = GetLocalPlayer())
		{
			if (auto Sub = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Sub->RemoveMappingContext(InputMappingContext);
				Sub->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
	else
	{
		if (ULocalPlayer* LP = GetLocalPlayer())
		{
			if (auto* Subsys = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsys->RemoveMappingContext(InputMappingContext);
			}
		}
	}
}

void ABangPlayerController::Server_OpenCamera_Implementation()
{
	if (!HasAuthority())
	{
		return;
	}
	ABangGameMode* GM = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (GM)
	{
		GM->OpenCamera(PlayerUniqueID);
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Yellow,
			FString::Printf(TEXT("[PlayerController :: ServerOpenCamera ]Player UniqueID : %d"), PlayerUniqueID)
		);
	}
}

void ABangPlayerController::Server_CloseCamera_Implementation()
{

	if (!HasAuthority())
	{
		return;
	}
	if (GetWorldTimerManager().IsTimerActive(BangModeTimerHandle))
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Red,
			TEXT("Bang Complete ,  Timer Clear")
		);
		GetWorldTimerManager().ClearTimer(BangModeTimerHandle);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Red,
			TEXT("Duration End")
		);
	}
	ABangGameMode* GM = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (GM)
	{
		GM->CloseCamera();
	}
}

void ABangPlayerController::Client_CloseCamera_Implementation()
{
	if (!IsLocalController())
	{
		return;
	}
	bIsCameraMode = false;

	if (ABangCharacter* BangPlayer = Cast<ABangCharacter>(GetPawn()))
	{
		
		UCameraComponent* StartCam = BangPlayer->BangCamera; 
		UCameraComponent* EndCam = BangPlayer->FollowCamera;
		if (!StartCam || !EndCam) return;


		const FTransform StartTransform = StartCam->GetComponentTransform();
		ACameraActor* TempCam = GetWorld()->SpawnActor<ACameraActor>(
			ACameraActor::StaticClass(), StartTransform);
		if (!TempCam) return;


		BangPlayer->FollowCamera->Deactivate();
		BangPlayer->BangCamera->Deactivate();

		constexpr float BlendTime = 5.f;
		CameraOpenBlendStartTime = FPlatformTime::Seconds();
		SetViewTarget(TempCam);

		const FVector StartLocation = StartTransform.GetLocation() - 100.f;
		const FVector EndLocation = EndCam->GetComponentLocation();

		const FVector FlagLocation = BangPlayer->GetFlagLocation();


		GetWorldTimerManager().SetTimer(CameraCloseBlendTimerHandle, FTimerDelegate::CreateLambda(
			[this, BangPlayer, TempCam, StartLocation, EndLocation, FlagLocation]() mutable
			{
				float Elapsed = FPlatformTime::Seconds() - CameraOpenBlendStartTime;
				float Alpha = FMath::Clamp(Elapsed / BlendTime, 0.f, 1.f);

				FVector NewLoc = FMath::Lerp(StartLocation, EndLocation, Alpha);
				TempCam->SetActorLocation(NewLoc);
				TempCam->SetActorRotation((FlagLocation - NewLoc).Rotation());

				if (Alpha >= 1.f)
				{
					//FTransform TempTransform = BangPlayer->GetInitialTransform();
					//사용자 움직이면 그냥 ㅈ대는 로직임 수정하긴해야하는데 
					//TempTransform.SetLocation(EndLocation);
					//CachedBangCameraTransform.SetRotation(TempTransform.GetRotation());
					//BangPlayer->FollowCamera->SetWorldTransform(CachedBangCameraTransform);
					/*BangPlayer->FollowCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
					BangPlayer->FollowCamera->AttachToComponent(BangPlayer->CameraBoom,
						FAttachmentTransformRules::SnapToTargetNotIncludingScale, USpringArmComponent::SocketName);*/
					//
					/*if (BangPlayer->CameraBoom)
					{
						BangPlayer->CameraBoom->SetRelativeTransform(BangPlayer->GetInitialBoomTransform());
					}
					BangPlayer->FollowCamera->SetRelativeTransform(BangPlayer->GetInitialCameraTransform());*/
					if (BangPlayer->GetFirstPersonMode())
					{
						BangPlayer->GetMesh()->SetVisibility(false);
					}
					BangPlayer->FollowCamera->Activate();
					SetViewTarget(BangPlayer);

					GetWorldTimerManager().ClearTimer(CameraCloseBlendTimerHandle);
					if (GetWorldTimerManager().IsTimerActive(CameraCloseBlendTimerHandle))
					{
						GEngine->AddOnScreenDebugMessage(
							-1,
							5.f,
							FColor::Red,
							TEXT("Error CloseTimer Acive")
						);
					}
					TempCam->Destroy();
				}
			}), 0.01f, true);
	}
}


void ABangPlayerController::Client_SelectTarget_Implementation(const uint32 TargetPlayerID)
{
	ABangPlayerHUD* BangHUD = Cast<ABangPlayerHUD>(GetHUD());
	UCardList * CardList = BangHUD->CardListWidgetInstance;

	ABangPlayerState* PS = GetPlayerState<ABangPlayerState>();
	if (!PS) return;
	FPlayerInformation* Myinfo = PS->PlayerInfo.GetPlayerInformation(PlayerUniqueID);

	if (UsingActiveType == EActiveType::Bang)
	{
		if (PS->PlayerInfo.IsBangDistanceAble(PlayerUniqueID, TargetPlayerID) || TargetPlayerID > 0 || bCanUseBang)
		{
			if (PS->CheckIsCardAbleByPassive(PlayerUniqueID, EPassiveType::Volcanic))
			{
				bCanUseBang = true;
				Server_UseCard(UsingCard, TargetPlayerID);
				PS->RestoreCard(PlayerUniqueID, UsingCard);
				PS->Server_SetPlayerInfo(PS->PlayerInfo);
				Myinfo->MyCards.RemoveCard(UsingCard.Card->SymbolType, UsingCard.Card->SymbolNumber);
				CardList->RemoveSelectedCard(UsingCard);
				InitializUsingCard();
			}
			else
			{
				bCanUseBang = false;
				Server_UseCard(UsingCard, TargetPlayerID);
				PS->RestoreCard(PlayerUniqueID, UsingCard);
				PS->Server_SetPlayerInfo(PS->PlayerInfo);
				Myinfo->MyCards.RemoveCard(UsingCard.Card->SymbolType, UsingCard.Card->SymbolNumber);
				CardList->RemoveSelectedCard(UsingCard);
				InitializUsingCard();
			}
		}
		else
		{
			//[사거리가 닿지 않습니다.]
			Client_RequestCardSelection(1, ECardSelectPurpose::UseCard);
			InitializUsingCard();
		}
	}
	else if (UsingActiveType == EActiveType::Robbery)
	{
		FPlayerInformation* Targetinfo = PS->PlayerInfo.GetPlayerInformation(TargetPlayerID);
		if (PS->PlayerInfo.IsDistanceAble(PlayerUniqueID, TargetPlayerID))
		{
			Server_UseCard(UsingCard, TargetPlayerID);
			PS->RestoreCard(PlayerUniqueID, UsingCard);
			PS->Server_SetPlayerInfo(PS->PlayerInfo);
			Myinfo->MyCards.RemoveCard(UsingCard.Card->SymbolType, UsingCard.Card->SymbolNumber);
			//Targetinfo->MyCards.RemoveCard(SelectCard->SymbolType, Select);

			CardList->RemoveSelectedCard(UsingCard);
			InitializUsingCard();
		}
	}
	else if (UsingActiveType == EActiveType::CatBalou ||
		UsingActiveType == EActiveType::Duel ||
		UsingActiveType == EActiveType::Jail)
	{
		Server_UseCard(UsingCard, TargetPlayerID);
		PS->RestoreCard(PlayerUniqueID, UsingCard);
		Myinfo->MyCards.RemoveCard(UsingCard.Card->SymbolType, UsingCard.Card->SymbolNumber);
		PS->Server_SetPlayerInfo(PS->PlayerInfo);
		CardList->RemoveSelectedCard(UsingCard);
		InitializUsingCard();
		
	}
}

void ABangPlayerController::HandleGeneralStoreSelectionComplete(const FSingleCard& SelectedCard)
{
	ABangPlayerState* PS = GetPlayerState<ABangPlayerState>();
	if (!PS) return;

	FPlayerInformation* MyInfo = PS->PlayerInfo.GetPlayerInformation(PlayerUniqueID);
	if (!MyInfo) return;

	// 선택한 카드 제거
	PS->PlayerInfo.SelectableCards.RemoveCard(SelectedCard.Card->SymbolType, SelectedCard.Card->SymbolNumber);

	// 다음 사람으로 MiniTurn 넘기기
	PS->MiniTurnUniqueID = PS->PlayerInfo.FindNextPlayer(PlayerUniqueID);

	PS->Server_SetPlayerInfo(PS->PlayerInfo);
}



void ABangPlayerController::Server_UseCard_Implementation(const FSingleCard& SingleCard, int32 TargetID)
{
	ABangPlayerState* PS = GetPlayerState<ABangPlayerState>();
	if (!PS) return;

	PS->UseCard(PlayerUniqueID, SingleCard, TargetID);
}

UCameraComponent* ABangPlayerController::FindCameraByTag(APawn* Player12, const FName& Tag)
{
	TArray<UCameraComponent*> BangCameras;
	Player12->GetComponents<UCameraComponent>(BangCameras);
	for (UCameraComponent* Cam : BangCameras)
	{
		if (Cam && Cam->ComponentHasTag(Tag))
		{
			return Cam;
		}
	}
	return nullptr;
}

void ABangPlayerController::Client_SetOutline_Implementation(uint32 OtherPlayerUniqueID, bool bEnable, int32 StencilValue)
{
	if (!IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("==================================================="));
		UE_LOG(LogTemp, Warning, TEXT("서버라 리턴합니다 !"));
		UE_LOG(LogTemp, Warning, TEXT("[[Outline] Input is %d , Has %d"), OtherPlayerUniqueID, PlayerUniqueID);
		UE_LOG(LogTemp, Warning, TEXT("==================================================="));
		return;
	}

	//여기에서 컨트롤러 아이디랑 비교하면될듯 
	UE_LOG(LogTemp, Warning, TEXT("==================================================="));
	UE_LOG(LogTemp, Warning, TEXT("[[Outline] Input is %d , Has %d"), OtherPlayerUniqueID,PlayerUniqueID);
	UE_LOG(LogTemp, Warning, TEXT("[Outline] NetMode=%d"), (int)GetNetMode());
	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Yellow,
			FString::Printf(TEXT("폰이 없어서 리턴됩니다 "))
		);
	}
	TArray<UMeshComponent*> Meshes;
	MyPawn->GetComponents<UMeshComponent>(Meshes);

	for (UMeshComponent* Mesh : Meshes)
	{
		Mesh->SetRenderCustomDepth(bEnable);
		Mesh->SetCustomDepthStencilValue(bEnable ? StencilValue : 0);
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Yellow,
			FString::Printf(TEXT("매쉬생성 완료 "))
		);
		UE_LOG(LogTemp, Warning, TEXT("[Outline] Mesh=%s Enabled=%d Stencil=%d"),
			*Mesh->GetName(),
			Mesh->bRenderCustomDepth,
			Mesh->CustomDepthStencilValue
		);
	}

	//if (!IsLocalController())
	//{
	//	GEngine->AddOnScreenDebugMessage(
	//		-1,
	//		5.0f,
	//		FColor::Yellow,
	//		FString::Printf(TEXT("서버라서..리턴됩니다 "))
	//	);
	//	UE_LOG(LogTemp, Error, TEXT("서버라서..리턴됩니다"));
	//	return;
	//}
	//
	
	/*ABangGameState* BangGameState = GetWorld()->GetGameState<ABangGameState>();
	if (!BangGameState) return;

	for (int i = 0; i < BangGameState->PlayerArray.Num(); i++)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Yellow,
			FString::Printf(TEXT("배열 순회중"))
		);
		if (ABangPlayerState* BangPS = Cast<ABangPlayerState>(BangGameState->PlayerArray[i]))
		{
			if (PlayerUniqueID != OtherPlayerUniqueID)
			{
				continue;
			}
			UE_LOG(LogTemp, Warning, TEXT("[SetOutline] find OtherPlayerUniqueID"));

			APawn* MyPawn = BangPS->GetPawn();
			if (!MyPawn)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					5.0f,
					FColor::Yellow,
					FString::Printf(TEXT("폰이 없어서 리턴됩니다 "))
				);
				continue;
			}
			TArray<UMeshComponent*> Meshes;
			MyPawn->GetComponents<UMeshComponent>(Meshes);

			for (UMeshComponent* Mesh : Meshes)
			{
				Mesh->SetRenderCustomDepth(bEnable);
				Mesh->SetCustomDepthStencilValue(bEnable ? StencilValue : 0);
				GEngine->AddOnScreenDebugMessage(
					-1,
					5.0f,
					FColor::Yellow,
					FString::Printf(TEXT("매쉬생성 완료 "))
				);
				UE_LOG(LogTemp, Warning, TEXT("[Outline] Mesh=%s Enabled=%d Stencil=%d"),
					*Mesh->GetName(),
					Mesh->bRenderCustomDepth,
					Mesh->CustomDepthStencilValue
				);
			}

		}
	}*/
	/*APawn* MyPawn = GetPawn();
	if (!MyPawn) return;

	TArray<UMeshComponent*> Meshes;
	MyPawn->GetComponents<UMeshComponent>(Meshes);

	for (UMeshComponent* Mesh : Meshes)
	{
		Mesh->SetRenderCustomDepth(bEnable);
		Mesh->SetCustomDepthStencilValue(bEnable ? StencilValue : 0);
	}*/

}

void ABangPlayerController::Client_ToggleMappingContext_Implementation()
{
	if (!IsLocalController())
	{
		return;
	}

	if (ULocalPlayer* LocalBangPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsys = LocalBangPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (bIsCameraContextActive)
			{
				Subsys->RemoveMappingContext(CameraMappingContext);
				Subsys->AddMappingContext(InputMappingContext, 0);
			}
			else
			{
				Subsys->RemoveMappingContext(InputMappingContext);
				Subsys->AddMappingContext(CameraMappingContext, 0);
			}
			bIsCameraContextActive = !bIsCameraContextActive;
		}
	}
}

//void ABangPlayerController::SetWidgetVisibility(uint32 PlayerID, bool bVisible)
void ABangPlayerController::SetWidgetVisibility(uint32 PlayerID, bool bVisible)
{

	if (!IsLocalController() || ControllerPlayerStateID == PlayerID)
		return;
	
	if (UWidgetComponent** CompPtr = PlayerWidgets.Find(PlayerID))
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Red,
			TEXT(" 위젯 찾았습니다  ")
		);
		UWidgetComponent* Comp = *CompPtr;
		Comp->SetVisibility(bVisible);
		Comp->SetHiddenInGame(!bVisible);
	}
}
void ABangPlayerController::GetUserInformationUI(uint32 BangPlayerStateID)
{
	//
}

void ABangPlayerController::GetPlayerStateAtBeginTest(uint32 BangPlayerStateID)
{
	
	if (!IsLocalController())
	{
		return;
	}
	if (PlayerWidgets.Contains(BangPlayerStateID))
	{
		return;
	}
	if (ABangCharacter* BangPlayer = Cast<ABangCharacter>(GetPawn()))
	{
		UWidgetComponent* WidgetComp = NewObject<UWidgetComponent>(BangPlayer);
		if (!WidgetComp)
		{
			return;
		}
		WidgetComp->SetupAttachment(BangPlayer->GetRootComponent());
		WidgetComp->RegisterComponent();

		WidgetComp->SetWidgetClass(InteractionWidgetClass);
		WidgetComp->InitWidget();
		WidgetComp->SetWidgetSpace(EWidgetSpace::World);
		WidgetComp->SetDrawSize({ 400, 200 });
		WidgetComp->SetRelativeLocation({ 0,0,BangPlayer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 50.f });
		WidgetComp->SetVisibility(false);
		WidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GEngine->AddOnScreenDebugMessage(
			-1,
			10.f,
			FColor::Red,
			TEXT("HAS Player State")
		);
		PlayerWidgets.Add(BangPlayerStateID, WidgetComp);
	}
}
void ABangPlayerController::Client_GetPlayerStateAtBeginTest_Implementation(uint32 BangPlayerStateID)
{
	//GetPlayerStateAtBeginTest(BangPlayerStateID);
}
void ABangPlayerController::Client_RemoveBangPlayerState_Implementation(uint32 BangPlayerStateID)
{
	//RemoveBangPlayerState(BangPlayerStateID);
}
void ABangPlayerController::RemoveBangPlayerState(uint32 BangPlayerStateID)
{
	if (!IsLocalController())
	{
		return;
	}

	UWidgetComponent** WidgetPtr = PlayerWidgets.Find(BangPlayerStateID);
	
	if (WidgetPtr && *WidgetPtr)
	{
		UWidgetComponent* WidgetComp = *WidgetPtr;
		WidgetComp->DestroyComponent();
		PlayerWidgets.Remove(BangPlayerStateID);
	}
}

/**
 * 특정 플레이어의 정보를 업데이트합니다. 지정된 고유 ID를 가진 플레이어의 HP 및 사거리를 갱신합니다.
 *
 * @param BangUniqueID 업데이트할 플레이어의 고유 ID입니다.
 * @param NewHP 업데이트할 플레이어의 새로운 HP 값입니다.
 * @param NewRange 업데이트할 플레이어의 새로운 사거리 값입니다.
 */
void ABangPlayerController::UpdatePlayerInfo(uint32 BangUniqueID, int32 NewHP, int32 NewRange)
{
	UWidgetComponent** WidgetCompPtr = PlayerWidgets.Find(BangUniqueID);
	if (!WidgetCompPtr)
	{
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("GetPlayerStateAtBegin 함수 종료  "));
	UWidgetComponent* WidgetComp = *WidgetCompPtr;
	if (!WidgetComp)
	{
		return;
	}

	UBangInfoWidget* InfoWidget = Cast<UBangInfoWidget>(WidgetComp->GetUserWidgetObject());
	if (!InfoWidget)
	{
		return;
	}

	//플레이어 인포 업데이트 하는 부분
	/*InfoWidget->UpdateRange(NewRange);
	InfoWidget->UpdateCurrentHealth(NewHP);*/
}


void ABangPlayerController::Client_UpdateGameLogUI_Implementation(const FString& GameLogMessage)
{

	if (ABangPlayerHUD* HUD = Cast<ABangPlayerHUD>(GetHUD()))
	{
		if (UPlayerListGameLog* StatusWidget = HUD->PlayerListGameLogInstance)
		{
			StatusWidget->AddGameLog(GameLogMessage);
		}
	}
}

void ABangPlayerController::Server_RequestPlayerListBroadcast_Implementation()
{
	if (ABangGameState* GS = GetWorld()->GetGameState<ABangGameState>())
	{
		GS->BroadcastPlayerListToClients();
	}
}


void ABangPlayerController::Client_UpdatePlayerListUI_Implementation(const TArray<FPlayerInformation>& PlayerList)
{
	UE_LOG(LogTemp, Warning, TEXT("[Client] PlayerListUI 업데이트 호출됨 - 플레이어 수: %d"), PlayerList.Num());

	if (ABangPlayerHUD* HUD = Cast<ABangPlayerHUD>(GetHUD()))
	{
		if (UPlayerListGameLog* StatusWidget = HUD->PlayerListGameLogInstance)
		{
			StatusWidget->UpdatePlayerList(PlayerList);
		}
	}
}


/**
 * 서버에서 카드를 뽑고 해당 내용을 로그에 기록하는 작업을 처리합니다.
 * 게임 모드 객체를 가져와 카드 뽑기 및 로그 작업을 수행합니다.
 */
void ABangPlayerController::Server_TestDrawCards_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT(" Server_TestDrawCards_Implementation() 실행"));
	if (ABangGameMode* GM = GetWorld()->GetAuthGameMode<ABangGameMode>())
	{
		// 여기서 카드 뽑고 로그 남기고 → 아래 클라이언트 함수 호출해야 함
	}
}

/**
 * 클라이언트에게 뽑은 카드들을 화면에 표시하는 작업을 수행합니다.
 * HUD 클래스에서 뽑은 카드 UI를 출력하는 함수 호출을 포함합니다.
 *
 * @param DrawnCards 클라이언트가 뽑은 카드들의 정보를 포함한 배열입니다.
 */
void ABangPlayerController::Client_ShowDrawnCards_Implementation()
{
	if (const TObjectPtr<ABangPlayerState> BangPlayerState = GetPlayerState<ABangPlayerState>())
	{
		FCardCollection OutCardCollection;
		BangPlayerState->GetSelectableCard(PlayerUniqueID, OutCardCollection);
		if (ABangPlayerHUD* HUD = Cast<ABangPlayerHUD>(GetHUD()))
		{
			HUD->ShowDrawCardUI(OutCardCollection);
		}
	}
}

/**
 * 서버에 게임 로그 전송 요청을 처리합니다.
 * 서버의 게임 모드에서 해당 요청을 승인하고 로그를 전송하는 작업을 수행합니다.
 */
void ABangPlayerController::Server_RequestSendGameLog_Implementation(const FString& GameLogMessage)
{
	if (ABangGameMode* GM = GetWorld()->GetAuthGameMode<ABangGameMode>())
	{
		GM->SendGameLog(GameLogMessage); 
	}
}
//void ABangPlayerController::LocalSetOutline(bool bEnable, int32 StencilValue)
//{
//	if ((int)GetNetMode() == 1)return;
//	UE_LOG(LogTemp, Warning, TEXT("==================================================="));
//	UE_LOG(LogTemp, Warning, TEXT("[Outline] NetMode=%d"), (int)GetNetMode());
//	APawn* MyPawn = GetPawn();
//	if (!MyPawn)
//	{
//		GEngine->AddOnScreenDebugMessage(
//			-1,
//			5.0f,
//			FColor::Yellow,
//			FString::Printf(TEXT("폰이 없어서 리턴됩니다 "))
//		);
//	}
//	TArray<UMeshComponent*> Meshes;
//	MyPawn->GetComponents<UMeshComponent>(Meshes);
//
//	for (UMeshComponent* Mesh : Meshes)
//	{
//		Mesh->SetRenderCustomDepth(bEnable);
//		Mesh->SetCustomDepthStencilValue(bEnable ? StencilValue : 0);
//		GEngine->AddOnScreenDebugMessage(
//			-1,
//			5.0f,
//			FColor::Yellow,
//			FString::Printf(TEXT("매쉬생성 완료 "))
//		);
//		UE_LOG(LogTemp, Warning, TEXT("[Outline] Mesh=%s Enabled=%d Stencil=%d"),
//			*Mesh->GetName(),
//			Mesh->bRenderCustomDepth,
//			Mesh->CustomDepthStencilValue
//		);
//	}
//}