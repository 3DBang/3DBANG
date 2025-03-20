#include "BangGameMode.h"

#include "GameState/BangGameState.h"
#include "PlayerState/BangPlayerState.h"
#include "PlayerController/BangPlayerController.h"
#include "../BangCharacter/BangCharacter.h"
ABangGameMode::ABangGameMode()
{
	DefaultPawnClass = ABangCharacter::StaticClass();
	PlayerControllerClass = ABangPlayerController::StaticClass();
}

void ABangGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&ABangGameMode::PrintPlayerStats,
		10.0f,
		true
	);
}

void ABangGameMode::PrintPlayerStats()
{
	// GameState의 PlayerArray에서 모든 플레이어의 PlayerState 접근
	if (AGameStateBase* GS = GetWorld()->GetGameState())
	{
		for (APlayerState* PlayerState : GS->PlayerArray)
		{
			if (ABangPlayerState* BangPlayerState = Cast<ABangPlayerState>(PlayerState))
			{
				BangPlayerState->PlayerStat.Value += 5;
				UE_LOG(LogTemp, Error, TEXT("Player %s stat: %d"),
					*BangPlayerState->GetPlayerName(),
					BangPlayerState->PlayerStat.Value);
				BangPlayerState->ForceNetUpdate();
				// 강제 없뎃 말고
				// 자연복제 주기 활용, 복제 설정 조정(NetUpdateFrequency) 또는 RPC로 직접 호출하여 적용해도 된다.
			}
		}
	}
}

void ABangGameMode::PostLogin(APlayerController* NewPlayer)
{
}
