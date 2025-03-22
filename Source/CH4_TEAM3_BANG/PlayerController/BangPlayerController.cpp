#include "BangPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Data/CardEnums.h"
#include "GameMode/BangGameMode.h"
#include "PlayerState/BangPlayerState.h"
#include "BangCharacter/BangCharacter.h"

#include "CharacterUIActor/BangUIActor.h"
#include "UI/BangInGameChattingWidget.h"
#include "UI/BangPlayerHUD.h"

ABangPlayerController::ABangPlayerController()
{}

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
	//여기에서 HasAuthoriy를 사용하면 서버이자 클라이언트는 Tick이 활성화가 되지 않는다
	if (IsLocalController())
	{
		SetActorTickEnabled(true);
	}
	else
	{
		SetActorTickEnabled(false);
	}
	/*FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(nullptr);
	SetInputMode(InputMode);*/
	bShowMouseCursor = true;
}

void ABangPlayerController::Server_UseCardReturn_Implementation(bool IsAble)
{
	
}

void ABangPlayerController::Server_EndTurn_Implementation(const uint32 UniqueID, ECharacterType PlayerCharacter)
{
	ABangGameMode* GM = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (GM)
	{
		GM->EndTurn(UniqueID, PlayerCharacter);
	}
}


void ABangPlayerController::Client_SetControllerRotation_Implementation(FRotator NewRotation)
{
	if (IsLocalController())
	{
		SetControlRotation(NewRotation);
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
    // UI 창 띄우기 (보유 중인 카드 표시)
    // 예시: UWidget* CardUI = CreateWidget<UWidget>(this, CardUIClass);
    // CardUI->AddToViewport();

    // 사용자가 카드 선택 (입력 대기)
    EActiveType SelectedActiveCard = EActiveType::None;
    EPassiveType SelectedPassiveCard = EPassiveType::None;

    // 카드 선택 후 처리 (별도 함수 호출)
    Client_HandleCardSelection(SelectedActiveCard);
}



void ABangPlayerController::Client_HandleCardSelection_Implementation(EActiveType SelectedCard)
{
    uint32 TargetPlayerID = 0; // 기본값, 상대가 필요하면 SelectTarget()에서 설정

    bool NeedsTarget = (SelectedCard == EActiveType::Bang ||
        SelectedCard == EActiveType::Robbery ||
        SelectedCard == EActiveType::CatBalou ||
        SelectedCard == EActiveType::Duel ||
        SelectedCard == EActiveType::Jail);

    if (NeedsTarget)
    {
        Client_SelectTarget();
        // 공격할 대상 선택 (레이 트레이싱 등)
        TargetPlayerID = 57;//SelectTarget();
        if (TargetPlayerID == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("대상이 필요한 카드인데 선택되지 않음!"));
            return;
        }
    }
    Server_UseCard(SelectedCard, TargetPlayerID);
}

///////////////////////////
//// 원명 추가 
//////////////////////////
void ABangPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ABangPlayerController::Client_SelectTarget_Implementation()
{
    uint32 TargetPlayerID = 15;//GetSelectedTargetID(); // 상대 플레이어 ID를 가져옴 (레이 트레이싱 담당자에게 받아올 부분)

    if (TargetPlayerID > 0)
    {

    }
}

void ABangPlayerController::Server_UseCard_Implementation(EActiveType SelectedCard, uint32 TargetPlayerID)
{
    ABangPlayerState* BangPlayerState = GetPlayerState<ABangPlayerState>();
    if (BangPlayerState)
    {
        //BangPlayerState->ProcessCardUsage(SelectedCard, TargetPlayerID);
    }
}

///////////////////////////
//// 찬호 추가 
//////////////////////////
void ABangPlayerController::Client_DisplayBangUI_Implementation()
{
	if (!HasAuthority())
	{
		if (const TObjectPtr<ABangPlayerHUD> BangHUD = Cast<ABangPlayerHUD>(GetHUD()))
		{
			BangHUD->ChattingWidgetInstance->AddMessage(
				FText::FromString(FString::Printf(TEXT("Hello from %d"), GetUniqueID())),
				FSlateColor(FLinearColor::Green)
			);
		}	
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

void ABangPlayerController::Server_StartGame_Implementation()
{
	const TObjectPtr<ABangGameMode> GameMode = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController] BeginPlay Controller GameMode is NULL!"));
		return;
	}

	GameMode->StartGame();
}

void ABangPlayerController::StartButtonCLicked()
{
	Server_StartGame();
}