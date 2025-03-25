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

#include "Data/PlayerInformation.h"
#include "UI/PlayerInfo/BangInfoWidget.h"

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


	if (IsLocalController())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABangPlayerController::GetPlayerStateAtBegin);
	}

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

void ABangPlayerController::Server_UseCardReturn_Implementation(bool IsAble)
{

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
	if (!PlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController::Client_OnTurnStart_Implementation]: PlayerState is null!"));
		return;
	}

	if (ABangPlayerHUD* BangHUD = Cast<ABangPlayerHUD>(GetHUD())) // HUD 캐스팅 및 유효성 검사
	{
		if (UCardList* CardListWidget = BangHUD->CardListWidgetInstance) // CardListWidgetInstance 유효성 검사
		{
			for (const FSingleCard& Card : DrawCards.CardList)
			{
				CardListWidget->AddCard(Card); // 카드 위젯 추가
			}

			// 허드에 있는 카드 사용 버튼 활성화
			// 턴종료 버튼 만들어서 활성화
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

void ABangPlayerController::Client_SelectCard_Implementation()
{
	// PlayerState에서 받아온 CurrentCardCollection을 기반으로
	// 유저가 실제로 들고 있는 카드 중에서 선택하도록 UI 연동

	const FCardCollection& MyCards = CurrentCardCollection;
	//이걸 기반으로 카드 UI에 표시하기
	
	// 유저가 클릭한 카드의 정보로
	//FSingleCard SelectedCard = /* 유저가 선택한 카드 */;
	//Client_HandleCardSelection(SelectedCard);
}


/**
 * 카드 선택에 대한 클라이언트 작업을 처리합니다. 선택된 카드와 필요에 따라 타겟 플레이어를 기반으로 적절한 서버 작업을 실행합니다.
 *
 * @param SingleCard 선택된 카드에 대한 정보를 포함하는 구조체입니다.
 */
void ABangPlayerController::Client_HandleCardSelection_Implementation(const FSingleCard& SingleCard)
{
    uint32 TargetPlayerID = 0; // 기본값, 상대가 필요하면 SelectTarget()에서 설정
	if (!SingleCard.Card)return;
	UE_LOG(LogTemp, Warning, TEXT("HandleCardSelection"));
	EActiveType OutActiveType;
	EPassiveType OutPassiveType;

	ABangPlayerState* PS = GetPlayerState<ABangPlayerState>();
	if (!PS) return;

	FPlayerInformation* Info = PS->PlayerInfo.GetPlayerInformation(PlayerUniqueID);
	if (!Info)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleCardSelection NONOinfo"));
		return;
	}

	PS->GetCardType(PlayerUniqueID, SingleCard, OutActiveType, OutPassiveType);
	UE_LOG(LogTemp, Warning, TEXT("OutActiveType: %s, OutPassiveType: %s"),
		*UEnum::GetValueAsString(OutActiveType),
		*UEnum::GetValueAsString(OutPassiveType));
	OutActiveType = EActiveType::Bang;
	OutPassiveType = EPassiveType::None;

	if (OutActiveType == EActiveType::Missed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Missed card cannot be used on your own turn"));
		return;
	}
	if (OutActiveType == EActiveType::Bang)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleCardSelection_INBang"));
		if (!bCanUseBang)return;
		if (Info->CharacterCardType == ECharacterType::WillyTheKid)
			//|| PS->CheckIsCardAble(PlayerUniqueID, ))
		{
			bCanUseBang = true;

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Bang!!Bang!!Bang!!Bang!!Bang!!"));
			bCanUseBang = false;
		}
	}
	bool bNeedsTarget = (OutActiveType == EActiveType::Robbery ||
		OutActiveType == EActiveType::CatBalou ||
		OutActiveType == EActiveType::Duel ||
		OutActiveType == EActiveType::Jail);

	if (bNeedsTarget)
	{
		Client_SelectTarget(SingleCard); // 나중에 실제 대상 선택 구현 예정

		if (TargetPlayerID == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Target required but not selected"));
			return;
		}
	}
	else
	{

		// 카드 사용
		Server_UseCard(SingleCard, TargetPlayerID);
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

	// 턴 종료 요청
	//PS->Server_EndTurn(UniqueID, MyInfo->CharacterCardType);
}

void ABangPlayerController::JCH_Test()
{
	ABangPlayerState* PS = GetPlayerState<ABangPlayerState>();
	if (PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerUniqueID : %d,"), PS->PlayerUniqueID);
	}
}

void ABangPlayerController::Client_RequestCardSelection_Implementation(
	int32 RequiredSelectCount,
	ECardSelectPurpose Purpose)
{
	UE_LOG(LogTemp, Warning, TEXT("[TEST] Client_RequestCardSelection_Implementation called"));
	switch (Purpose)
	{
	case ECardSelectPurpose::UseCard:
	{
		//카드 사용하기
		//OnUseInputButtonClicked(사용하기, 1,  ECardSelectPurpose::UseCard:)
		break;
	}

	case ECardSelectPurpose::DiscardCard:
		// 보유 카드 수 > 체력, 초과분 만큼 버려야 함
		//버튼을 버리기 버튼으로 바꿈 
		//OnUseInputButtonClicked(버리기, 보유카드 - 현재체력, ECardSelectPurpose::DiscardCard)
		break;

	case ECardSelectPurpose::GeneralStoreDraft:
		// 잡화점 – 전체 플레이어가 순서대로 카드 중 1장 선택
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
		// 결투 중 뱅 카드 선택
		// 보유 카드 띄우기 1장 선택(뱅만)
		//OnUseInputButtonClicked(응수하기, 1, ECardSelectPurpose::RespondToDuel:)
		break;

	case ECardSelectPurpose::RespondToIndians:
		// 인디언 카드 대응 – 뱅 카드 선택
		// 보유 카드 중 1장 선택(뱅만)
		//OnUseInputButtonClicked(쫓아내기, 1, ECardSelectPurpose::RespondToIndians:)
		break;

	case ECardSelectPurpose::RespondToAttack:
		// Bang, Gatling 등의 공격에 대해 Missed 카드 선택
		// 보유카드 중 1장 선택(Missed)만
		//OnUseInputButtonClicked(피하기, 1, ECardSelectPurpose::RespondToAttack:)
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Unknown Card Select Purpose"));
		break;
	}
}

void ABangPlayerController::OnCardSelectionComplete(
	const TArray<FSingleCard>& SelectedCards,       // 플레이어가 실제로 선택한 카드들
	int32 RequiredSelectCount,                      // 선택해야 할 개수
	ECardSelectPurpose Purpose)                     // 선택 목적
{
	UE_LOG(LogTemp, Warning, TEXT("SelectedCards.Num(): %d, RequiredCount: %d"), SelectedCards.Num(), RequiredSelectCount);
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


	bool bAllowEmptySelection = (
		Purpose == ECardSelectPurpose::RespondToDuel ||
		Purpose == ECardSelectPurpose::RespondToIndians ||
		Purpose == ECardSelectPurpose::RespondToAttack
		);

	if (!bAllowEmptySelection && SelectedCards.Num() != RequiredSelectCount)return;


	switch (Purpose)
	{
	case ECardSelectPurpose::UseCard:
	{
		//카드 사용하기
		UE_LOG(LogTemp, Warning, TEXT("[TEST]OnCardSelectionComplete UseCard"));
		Client_HandleCardSelection(SelectedCards[0]);
		break;
	}
	case ECardSelectPurpose::DiscardCard:
	{
		for (const FSingleCard& Card : SelectedCards)
		{
			//보유 카드에서 제거 후 버린카드덱에 추가
			MyInfo->MyCards.RemoveCard(Card.Card->SymbolType, Card.Card->SymbolNumber);
			PS->RestoreCard(PlayerUniqueID, Card);
			PS->Server_SetPlayerInfo(PS->PlayerInfo);
		}

		// 턴 종료 호출
		PS->Server_EndTurn(PlayerUniqueID);
		break;
	}

	case ECardSelectPurpose::GeneralStoreDraft:
		// 잡화점 – 전체 플레이어가 순서대로 카드 중 1장 선택
		//MyInfo.SelectableCards 삭제 선택한 카드 보유카드에 추가.
		break;

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

		FCardCollection SelectedCardCollection;
		SelectedCardCollection.CardList = SelectedCards;
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
		}
		break;
	}

	case ECardSelectPurpose::StealFromOpponent:
	{// 상대의 보유 카드 중 1장을 선택 
		// 상대 카드 중 1장 없애기
		// 내 카드 덱에 1장 추가하기
		break;
	}
	case ECardSelectPurpose::RespondToDuel:
	{// 결투 중 뱅 카드 선택
		PS->GetCardType(PlayerUniqueID, SelectedCards[0], OutActiveType, OutPassiveType);
		if(OutActiveType == EActiveType::Bang)
		{
			// 뱅 카드 사용(결투 반격 성공)
			// 카드 지우기
			PS->RestoreCard(PlayerUniqueID, SelectedCards[0]);
		}
		else
		{
			//잘못된 카드 사용 처리
		}
		break;
	}
	case ECardSelectPurpose::RespondToIndians:
	{// 인디언 카드 대응 – 뱅 카드 선택
		PS->GetCardType(PlayerUniqueID, SelectedCards[0], OutActiveType, OutPassiveType);
		if (OutActiveType == EActiveType::Bang)
		{
			// 뱅 카드 사용(인디언 쫓아내기 성공)
			PS->RestoreCard(PlayerUniqueID, SelectedCards[0]);
		}
		else
		{
			//잘못된 카드 사용 처리
		}
		break;
	}

	case ECardSelectPurpose::RespondToAttack:
	{	// Bang, Gatling 등의 공격에 대해 Missed 카드 선택
		PS->GetCardType(PlayerUniqueID, SelectedCards[0], OutActiveType, OutPassiveType);
		if (OutActiveType == EActiveType::Missed)
		{
			// 회피 카드 사용(회피 성공)
			PS->RestoreCard(PlayerUniqueID, SelectedCards[0]);
		}
		else
		{
			//잘못된 카드 사용 처리
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

void ABangPlayerController::TestButtonCLicked()
{
	UE_LOG(LogTemp, Error, TEXT("TestButtonCLicked"));
	// 로직 작성
	
	Server_StartTest();
	Server_RequestPlayerListBroadcast();
	Server_TestDrawCards();
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
	FHitResult HitResult;
	if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, HitResult))
	{
		DrawDebugSphere(GetWorld(), HitResult.Location, 10.f, 8, FColor::Red, false, 1.5f);
		ACharacter* HitChar = Cast<ACharacter>(HitResult.GetActor());
		
		
		if (HitChar && HitChar != GetPawn())
		{

			if (ABangCharacter* OtherPlayer = Cast<ABangCharacter>(HitChar))
			{
				

				CurrentMouseCursor = EMouseCursor::Hand;
				if (bIsCameraMode)
				{
					//uint32 GetUID = GetUniqueID();
					//여기를 수정해야함 
					//SendToServer And Send CloseCamera Request
					//TODO : SendToServerMethod()
					Server_CloseCamera();

				}

				else
				{
					// === 위젯 생성 및 표시 ===
					if (InteractionWidgetClass)
					{
						
						ABangPlayerController* PCTest = Cast<ABangPlayerController>(GetWorld()->GetFirstPlayerController());
						if (PCTest && PCTest->PlayerState)
						{
							ABangPlayerState* PSTest = Cast<ABangPlayerState>(PCTest->PlayerState);
							if (PSTest)
							{
								GEngine->AddOnScreenDebugMessage(
									-1,                              // Key: -1 = auto‑generate a new message each call
									5.0f,                            // Duration (seconds)
									FColor::Yellow,                  // Text color
									FString::Printf(TEXT("PS TEST PlayerUniqueID: %d"), PSTest->PlayerUniqueID)
								);
							}
						}


						ABangPlayerState* BangState = Cast<ABangPlayerState>(OtherPlayer->GetPlayerState());
						if (GEngine)
						{
							auto Information = BangState->PlayerInfo.GetPlayerInformation(BangState->PlayerUniqueID);
							UE_LOG(LogTemp, Display, TEXT("Unique id %d"), Information->PlayerUniqueID);
							UE_LOG(LogTemp, Display, TEXT("Current Hea%d"), Information-> CurrentHealth);
							UE_LOG(LogTemp, Display, TEXT("Range %d"), Information->Range);
							UE_LOG(LogTemp, Display, TEXT("Name is %s"), *Information->PlayerName);
							UE_LOG(LogTemp, Display, TEXT("====================="));
						}
						if (PlayerWidgets.Contains(BangState->PlayerUniqueID))
						{
							
							//OtherPlayer->GetPlayerState()->GetPlayerId())
							/*GEngine->AddOnScreenDebugMessage(
								-1,
								10.f,
								FColor::Red,
								TEXT("[Mouse Click ]HAS Player State")
							);*/
							//InteractionWidgetComponent = *PlayerWidgets.Find(OtherPlayer->GetPlayerState()->GetPlayerId());

						}
						else
						{
							GEngine->AddOnScreenDebugMessage(
								-1,
								10.f,
								FColor::Red,
								TEXT("[Mouse Click ] No Player State")
							);
							InteractionWidgetComponent = NewObject<UWidgetComponent>(OtherPlayer);
							InteractionWidgetComponent->SetupAttachment(OtherPlayer->GetRootComponent());
							InteractionWidgetComponent->RegisterComponent();
							InteractionWidgetComponent->SetWidgetClass(InteractionWidgetClass);
							InteractionWidgetComponent->InitWidget();

							InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
							InteractionWidgetComponent->SetDrawSize(FVector2D(400, 200));
							InteractionWidgetComponent->SetRelativeLocation(
								FVector(0.f, 0.f, OtherPlayer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 50.f)
							);

							PlayerWidgets.Add(BangState->PlayerUniqueID, InteractionWidgetComponent);

						}
						if (InteractionWidgetComponent)
						{
							InteractionWidgetComponent->SetVisibility(true);
							InteractionWidgetComponent->SetHiddenInGame(false);
						}
					}
				
					/**Test*/
					uint32 PlayerStateID = 0;
					uint32 TestTemp = 0;
					GEngine->AddOnScreenDebugMessage(
						-1,
						5.f,
						FColor::Red,
						TEXT("This is a debug message!")
					);
					if (OtherPlayer->GetPlayerState())
					{
						PlayerStateID = OtherPlayer->GetPlayerState()->GetPlayerId();
						UE_LOG(LogTemp, Log, TEXT("PlayerStateID = %d"), PlayerStateID);
						
						ABangPlayerState* PlayerBangState = Cast<ABangPlayerState>(OtherPlayer->GetPlayerState());
						if (PlayerBangState)
						{
							//Get Information for UI
							//And Open UI
						}
					}
				}
			}
		}

	}
	else
	{
		//CloseHuD 
	}
	CurrentMouseCursor = EMouseCursor::Default;
}

void ABangPlayerController::Client_OpenCamera_Implementation()
{
	if (!IsLocalController())
	{
		return;
	}



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
		const FVector EndLocation = EndCam->GetComponentLocation() + 300.f; // 마지막에 회전하는 효과를 주고 싶어서 벡터를 사용해서 300f만큼 이동 그러면 마지막에 꿀벌마냥 회전할것

		//BangCamera의 위치를 한번 봐야할듯
		const FVector FlagLocation = BangPlayer->GetFlagLocation();

		GetWorldTimerManager().SetTimer(CameraOpenBlendTimerHandle, FTimerDelegate::CreateLambda(
			[this, BangPlayer, TempCam, StartLocation, EndLocation, FlagLocation]() mutable
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
		GetWorldTimerManager().SetTimer(BangModeTimerHandle, this, &ABangPlayerController::Server_CloseCamera, 10.f, false);
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

	//여기도 바꿔야하네 
	uint32 BangUID = GetUniqueID();
	//게임모드에서도 바꿔야하고 이거 

	ABangGameMode* GM = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (GM)
	{
		GM->OpenCamera(BangUID);
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

void ABangPlayerController::Client_SelectTarget_Implementation(const FSingleCard& SingleCard)
{
	uint32 TargetPlayerID = 15;//GetSelectedTargetID(); // 상대 플레이어 ID를 가져옴 (레이 트레이싱 담당자에게 받아올 부분)

    if (TargetPlayerID > 0)
    {
		Server_UseCard(SingleCard, TargetPlayerID);
		UE_LOG(LogTemp, Warning, TEXT("The End"));
    }
}

void ABangPlayerController::Server_UseCard_Implementation(const FSingleCard& SingleCard, int32 TargetID)
{
	ABangPlayerState* PS = GetPlayerState<ABangPlayerState>();
	if (!PS) return;

	const int32 FromID = PS->GetUniqueID(); // 혹은 PlayerState에 저장된 내 ID
	PS->UseCard(FromID, SingleCard, TargetID);
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

void ABangPlayerController::Client_SetOutline_Implementation(bool bEnable, int32 StencilValue)
{
	if (!IsLocalController())
		return;

	//이거 그냥 동기화말고 서버에서 뿌려줘서 해당하는 애 찾아버리자 
	//추가 제거함수 
	APawn* MyPawn = GetPawn();
	if (!MyPawn) return;

	TArray<UMeshComponent*> Meshes;
	MyPawn->GetComponents<UMeshComponent>(Meshes);

	for (UMeshComponent* Mesh : Meshes)
	{
		Mesh->SetRenderCustomDepth(bEnable);
		Mesh->SetCustomDepthStencilValue(bEnable ? StencilValue : 0);
	}
}

///////////////////////////
//// 찬호 추가 
//////////////////////////

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

	InfoWidget->UpdateRange(NewRange);
	InfoWidget->UpdateCurrentHealth(NewHP);
}

void ABangPlayerController::Server_RequestPlayerListBroadcast_Implementation()
{
	if (ABangGameState* GS = GetWorld()->GetGameState<ABangGameState>())
	{
		GS->BroadcastPlayerListToClients();
	}
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

void ABangPlayerController::Client_UpdatePlayerListUI_Implementation(const TArray<FPlayerInformation>& PlayerList)
{
	if (ABangPlayerHUD* HUD = Cast<ABangPlayerHUD>(GetHUD()))
	{
		if (UPlayerListGameLog* StatusWidget = HUD->PlayerListGameLogInstance)
		{
			StatusWidget->UpdatePlayerList(PlayerList);
		}
	}
}

void ABangPlayerController::Server_TestDrawCards_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT(" Server_TestDrawCards_Implementation() 실행"));
	if (ABangGameMode* GM = GetWorld()->GetAuthGameMode<ABangGameMode>())
	{
		GM->Test_DrawAndLogCards(); // 여기서 카드 뽑고 로그 남기고 → 아래 클라이언트 함수 호출해야 함
	}
}

void ABangPlayerController::Client_ShowDrawnCards_Implementation(const TArray<FSingleCard>& DrawnCards)
{
	if (ABangPlayerHUD* HUD = Cast<ABangPlayerHUD>(GetHUD()))
	{
		HUD->ShowDrawCardUI(DrawnCards);
	}
}
