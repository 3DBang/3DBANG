#include "BangPlayerController.h"
#include "EnhancedInputSubsystems.h"

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

void ABangPlayerController::Server_UseCardReturn_Implementation(bool IsAble)
{
	
}

void ABangPlayerController::Server_EndTurn_Implementation()
{
	ABangGameModeBase* GM = GetWorld()->GetAuthGameMode<ABangGameModeBase>();
	if (GM)
	{
		GM->EndTurn();
	}
}

/*void ABangPlayerController::Server_AttackPlayer_Implementation(APlayerState* TargetPlayer)
{
	ABangGameModeBase* GM = GetWorld()->GetAuthGameMode<ABangGameModeBase>();
	if (GM)
	{
		GM->HandleAttack(GetPlayerState<ABangPlayerState>(), Cast<ABangPlayerState>(TargetPlayer));
	}
}*/
void ABangPlayerController::Client_SetControllerRotation_Implementation(FRotator NewRotation)
{
	if (IsLocalController())
	{
		SetControlRotation(NewRotation);
	}
	
}
