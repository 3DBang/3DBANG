#include "BangPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Data/CardEnums.h"
#include "GameMode/BangGameMode.h"

ABangPlayerController::ABangPlayerController()
{
	UE_LOG(LogTemp, Error, TEXT("플레이어 컨트롤러가 생성되었습니다 순서는 누가 더 빠른가요?"));
}

void ABangPlayerController::BeginPlay()
{
	Super::BeginPlay();

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

    if (ABangPlayerState* PS = GetPlayerState<ABangPlayerState>())
    {
        if (PS)
        {
            MyPlayerID = PS->GetPlayerId();
           //MyJobType = PS->GetJobType();
           //MyCharacterType = PS->GetCharacterType();
        }
    }
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

void ABangPlayerController::Server_UseCard_Implementation(uint32 UniqueID, ECardType CardType, EActiveType ActiveType, EPassiveType PassiveType, uint32 ToUniqueID)
{
    ABangGameMode* GameMode = GetWorld()->GetAuthGameMode<ABangGameMode>();
    if (GameMode)
    {
        GameMode->UseCard(UniqueID, CardType, ActiveType, PassiveType, ToUniqueID);
    }
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
    const uint32 FromID = MyPlayerID;
    const uint32 ToID = 0; // 기본값, 상대가 필요하면 SelectTarget()에서 설정

    switch (SelectedActiveCard)
    {
    case EActiveType::Missed:
    case EActiveType::Stagecoach:
    case EActiveType::WellsFargoBank:
    case EActiveType::Beer:
    case EActiveType::GatlingGun:
    case EActiveType::Saloon:
    case EActiveType::GeneralStore:
    case EActiveType::Indians:
    case EActiveType::Dynamite:
        // 서버에 카드 사용 요청 보내기
        Server_UseCard(FromID, ECardType::ActiveCard, SelectedActiveCard, SelectedPassiveCard, ToID);
        break;
    case EActiveType::Bang:
    case EActiveType::Robbery:
    case EActiveType::CatBalou:
    case EActiveType::Duel:
    case EActiveType::Jail:
        // 상대방이 필요한 경우, 상대방 ID 가져오기
        Client_SelectTarget();
        Server_UseCard(FromID, ECardType::ActiveCard, SelectedActiveCard, SelectedPassiveCard, ToID);
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("잘못된 카드 선택"));
        break;
    }
}

void ABangPlayerController::Client_SelectTarget_Implementation()
{
    uint32 TargetPlayerID = 15;//GetSelectedTargetID(); // 상대 플레이어 ID를 가져옴 (레이 트레이싱 담당자에게 받아올 부분)

    if (TargetPlayerID > 0)
    {
        Server_UseCard(MyPlayerID, ECardType::ActiveCard, EActiveType::Bang, EPassiveType::None, TargetPlayerID);
    }
}
