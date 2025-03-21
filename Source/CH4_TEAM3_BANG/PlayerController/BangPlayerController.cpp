#include "BangPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Data/CardEnums.h"
#include "GameMode/BangGameMode.h"
#include "PlayerState/BangPlayerState.h"
#include "BangCharacter/BangCharacter.h"
#include "CharacterUIActor/BangUIActor.h"

//Team_State
#include "Data/BangPlayerStatData.h"
#include "UI/BangInGameChattingWidget.h"
#include "UI/BangInGamePlayerListWidget.h"
#include "UI/BangPlayerHUD.h"

ABangPlayerController::ABangPlayerController()
{
	UE_LOG(LogTemp, Error, TEXT("플레이어 컨트롤러가 생성되었습니다 순서는 누가 더 빠른가요?"));
}

void ABangPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Error, TEXT("ABangPlayerController BeginPlay"));

	// 순서 변경 필요
	if (IsLocalController())
	{
		if (const TObjectPtr<ABangPlayerHUD> BangHUD = Cast<ABangPlayerHUD>(GetHUD()))
		{
			BangHUD->ChattingWidgetInstance->AddMessage(FText::FromString("Hello from Controller!"), FSlateColor(FLinearColor::Green));

			TArray<UBangPlayerStatData*> PlayerStats;

			const auto Stat = NewObject<UBangPlayerStatData>();
			Stat->PlayerId = "PlayerOne";
			Stat->bIsAlive = true;
			PlayerStats.Add(Stat);
			
			BangHUD->PlayerListWidgetInstance->UpdatePlayerList(PlayerStats);
		}
	}
	// 순서 변경 필요
	
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

void ABangPlayerController::Server_UseCardReturn_Implementation(bool IsAble)
{
	
}

void ABangPlayerController::Server_EndTurn_Implementation()
{
    ABangPlayerState* PS = GetPlayerState<ABangPlayerState>();

    if (!PS)
    {
        UE_LOG(LogTemp, Error, TEXT("[PlayerController] PlayerState를 찾을 수 없습니다!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[PlayerController] PlayerState에게 턴 종료 요청 전송"));
    //PlayerState->HandleTurnEnd();
}


void ABangPlayerController::Client_SetControllerRotation_Implementation(FRotator NewRotation)
{
	if (IsLocalController())
	{
		SetControlRotation(NewRotation);
	}	
}

void ABangPlayerController::Client_SelectCard_Implementation()
{
    // UI 창 띄우기 (보유 중인 카드 표시)
    // 예시: UWidget* CardUI = CreateWidget<UWidget>(this, CardUIClass);
    // CardUI->AddToViewport();

    // 사용자가 카드 선택 (입력 대기)
    EActiveType SelectedActiveCard = EActiveType::None;
    EPassiveType SelectedPassiveCard = EPassiveType::None;

    // 카드 선택 후 처리 (별도 함수 호출)
    Client_HandleCardSelection(SelectedActiveCard, SelectedPassiveCard);
}



void ABangPlayerController::Client_HandleCardSelection_Implementation(EActiveType SelectedActiveCard, EPassiveType SelectedPassiveCard)
{
    uint32 TargetPlayerID = 0; // 기본값, 상대가 필요하면 SelectTarget()에서 설정

    //패시브 카드라면 상대를 지목할 필요 없음
    if (SelectedPassiveCard != EPassiveType::None)
    {
        // 패시브 카드는 그냥 사용
        //Server_UseCard(SelectedActiveCard, TargetPlayerID);
        return;
    }

    //특정 Active 카드들만 대상 플레이어가 필요함.
    bool NeedsTarget = (SelectedActiveCard == EActiveType::Bang ||
        SelectedActiveCard == EActiveType::Robbery ||
        SelectedActiveCard == EActiveType::CatBalou ||
        SelectedActiveCard == EActiveType::Duel ||
        SelectedActiveCard == EActiveType::Jail);

    if (NeedsTarget)
    {
        // 대상 선택 UI 표시 (또는 레이 트레이싱 등)
        // 다같이 카메라 위로 해줘()
        Client_SelectTarget();
        UE_LOG(LogTemp, Log, TEXT("TargetSelect"));
        //테스트용 임시 타겟 (실제는 GetSelectedTargetID() 등으로 구현)
        TargetPlayerID = 57;

        if (TargetPlayerID == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("대상이 필요한 카드인데 선택되지 않음!"));
            return;
        }
    }
    //서버에 카드 사용 요청 보내기
    Server_UseCard(SelectedActiveCard, SelectedPassiveCard, TargetPlayerID);
}

void ABangPlayerController::Client_SelectTarget_Implementation()
{
    uint32 TargetPlayerID = 15;//GetSelectedTargetID(); // 상대 플레이어 ID를 가져옴 (레이 트레이싱 담당자에게 받아올 부분)

    if (TargetPlayerID > 0)
    {

    }
}

void ABangPlayerController::Server_UseCard_Implementation(EActiveType SelecteSelectedActiveCard, EPassiveType SelectedPassiveCard, uint32 TargetPlayerID)
{
    ABangPlayerState* BangPlayerState = GetPlayerState<ABangPlayerState>();
    if (BangPlayerState)
    {
        //BangPlayerState->ProcessCardUsage(SelectedCard, TargetPlayerID);
    }
}

void ABangPlayerController::ShowDiscardUI_Implementation(const TArray<EActiveType>& ActiveCards, const TArray<EPassiveType>& PassiveCards)
{/*
    UE_LOG(LogTemp, Log, TEXT("[PlayerController] 카드 버리기 UI를 띄웁니다."));

    if (!DiscardUIWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("[PlayerController] DiscardUI 위젯이 설정되지 않았습니다!"));
        return;
    }

    UDiscardUI* DiscardUI = CreateWidget<UDiscardUI>(this, DiscardUIWidget);
    if (!DiscardUI)
    {
        UE_LOG(LogTemp, Error, TEXT("[PlayerController] 카드 버리기 UI 생성 실패!"));
        return;
    }

    // 🔥 보유 중인 카드 목록을 UI에 전달
    DiscardUI->SetupDiscardUI(ActiveCards, PassiveCards);

    // 🔥 UI 표시
    DiscardUI->AddToViewport();*/
}



void ABangPlayerController::Server_DiscardCards_Implementation(const TArray<EActiveType>& DiscardedActiveCards, const TArray<EPassiveType>& DiscardedPassiveCards)
{
    ABangPlayerState* PS = GetPlayerState<ABangPlayerState>();

    if (!PS)
    {
        UE_LOG(LogTemp, Error, TEXT("[PlayerController] PlayerState를 찾을 수 없습니다!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[PlayerController] 유저가 버릴 카드 선택 - 액티브 %d장, 패시브 %d장"),
        DiscardedActiveCards.Num(), DiscardedPassiveCards.Num());

    // PlayerState로 카드 버리기 요청 전송
    //PlayerState->HandleDiscardCards(DiscardedActiveCards, DiscardedPassiveCards);
}

