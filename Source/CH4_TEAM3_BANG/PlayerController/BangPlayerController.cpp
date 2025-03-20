#include "BangPlayerController.h"
#include "EnhancedInputSubsystems.h"

ABangPlayerController::ABangPlayerController()
{
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
