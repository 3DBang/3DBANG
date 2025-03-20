#include "BangPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Card/Data/CardEnums.h"

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
}

void ABangPlayerController::Client_SetControllerRotation_Implementation(FRotator NewRotation)
{
	if (IsLocalController())
	{
		SetControlRotation(NewRotation);
	}
	
}

/*
void ABangPlayerController::Client_ShowOwnedCards_Implementation()
{
    // UI 창 띄우기 (보유 중인 카드 표시)
    // 예시: UWidget* CardUI = CreateWidget<UWidget>(this, CardUIClass);
    // CardUI->AddToViewport();

    // 사용자가 카드 선택 (입력 대기)
    EActiveType SelectedCard = EActiveType::None;

    // 입력값 처리
    switch (SelectedCard)
    {
    case EActiveType::Bang:
        // 공격 카드 사용 -> 공격할 대상 선택 UI 열기
        SelectTarget();
        break;

    case EActiveType::Missed:
        // 회피 카드 사용 -> 서버에 바로 전달
        UseCard(SelectedCard);
        break;

    case EActiveType::Stagecoach:
        // 카드 2장 뽑기 (서버에 요청)
        UseCard(SelectedCard);
        break;

    case EActiveType::WellsFargoBank:
        // 카드 3장 뽑기 (서버에 요청)
        UseCard(SelectedCard);
        break;

    case EActiveType::Beer:
        // 체력 회복 (서버에 요청)
        UseCard(SelectedCard);
        break;

    case EActiveType::GatlingGun:
        // 전원 공격 (서버에 요청)
        UseCard(SelectedCard);
        break;

    case EActiveType::Robbery:
        // 다른 플레이어의 카드 강탈 (대상 선택 필요)
        SelectTarget();
        break;

    case EActiveType::CatBalou:
        // 다른 플레이어의 카드 버리기 (대상 선택 필요)
        SelectTarget();
        break;

    case EActiveType::Saloon:
        // 모든 플레이어 체력 회복 (서버에 요청)
        UseCard(SelectedCard);
        break;

    case EActiveType::Duel:
        // 결투 (뱅을 연속으로 내야 함)
        SelectTarget();
        break;

    case EActiveType::GeneralStore:
        // 모든 플레이어가 카드 한 장 가져감 (서버에 요청)
        UseCard(SelectedCard);
        break;

    case EActiveType::Indians:
        // 모든 플레이어가 뱅을 내야 함 (서버에 요청)
        UseCard(SelectedCard);
        break;

    case EActiveType::Jail:
        // 감옥 (턴 제한) (서버에 요청)
        UseCard(SelectedCard);
        break;

    case EActiveType::Dynamite:
        // 다이너마이트 (폭발 시 피해) (서버에 요청)
        UseCard(SelectedCard);
        break;

    default:
        // 기타 카드 처리 (없으면 무시)
        break;
    }
}
*/

