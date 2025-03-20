#include "BangGameMode.h"

#include "GameState/BangGameState.h"
#include "PlayerState/BangPlayerState.h"
#include "PlayerController/BangPlayerController.h"
#include "../BangCharacter/BangCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Algo/Shuffle.h"
#include "GameFramework/PlayerStart.h"

ABangGameMode::ABangGameMode()
{
	DefaultPawnClass = ABangCharacter::StaticClass();
	PlayerControllerClass = ABangPlayerController::StaticClass();
    bDelayedStart = true;

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
	Super::PostLogin(NewPlayer);
	PlayerControllers.Add(NewPlayer);
	//게임 시작버튼을 누르면 그때 Player위치 조정함수 사용
	//현재는 테스트용 입니다 
	//SpawnPlayers();
}



void ABangGameMode::SpawnPlayers()
{
    TObjectPtr<APlayerStart> BasePlayerStart = ChooseStartLocation();
    if (!BasePlayerStart)
    {
        UE_LOG(LogTemp, Warning, TEXT("No PlayerStart found to use as center."));
        return;
    }
    
    UE_LOG(LogTemp, Error, TEXT("Player Start with Flagged is  : %s"), *BasePlayerStart->GetActorLocation().ToString());


    FVector Center = BasePlayerStart->GetActorLocation();

    int32 PlayersNum = PlayerControllers.Num();
    UE_LOG(LogTemp, Error, TEXT("Player Num is %d"),PlayersNum);
    if (PlayersNum <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No PlayerControllers available for spawning."));
        return;
    }

 

    for (int32 i = 0; i < PlayersNum; i++)
    {
        float Radian = (2 * PI / PlayersNum) * i;
        FVector Offset(FMath::Cos(Radian) * Radius, FMath::Sin(Radian) * Radius, 0.f);
        FVector SpawnLocation = Center + Offset;
        FRotator SpawnRotation = (Center - SpawnLocation).Rotation();
        FRotator DefaultRotation = DefaultPawnClass->GetDefaultObject<ABangCharacter>()->GetActorRotation();
        SpawnRotation -= DefaultRotation;

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = PlayerControllers[i];
        SpawnParams.Instigator = PlayerControllers[i]->GetPawn();

        ABangCharacter* Player = GetWorld()->SpawnActor<ABangCharacter>(DefaultPawnClass, SpawnLocation, SpawnRotation, SpawnParams);
        if (Player)
        {
            PlayerControllers[i]->Possess(Player);
            UE_LOG(LogTemp, Error, TEXT("플레이어컨트롤러가 폰에 빙의했습니다."));
           
            if (ABangPlayerController* PlayerController = Cast<ABangPlayerController>(PlayerControllers[i]))
            {
                PlayerController->Client_SetControllerRotation(SpawnRotation);
            }
           //TODO : Suffle

            UE_LOG(LogTemp, Error, TEXT("Player Controller is %s"), *PlayerControllers[i]->GetName());

            UE_LOG(LogTemp, Error, TEXT("Spawn Location is : %s"), *SpawnLocation.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to spawn Pawn for player index %d"), i);
        }
    }
}
APlayerStart* ABangGameMode::ChooseStartLocation() const
{
    TArray<AActor*> AllPlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), AllPlayerStarts);

    APlayerStart* FlaggedStarts;
    for (AActor* Actor : AllPlayerStarts)
    {
        APlayerStart* Start = Cast<APlayerStart>(Actor);
        if (Start && Start->ActorHasTag(FName("Flagged")))
        {
            FlaggedStarts = Start;
            return FlaggedStarts;
        }
    }
    return nullptr;
}

void ABangGameMode::SpawnPlayerBlue()
{
    SpawnPlayers();
}

