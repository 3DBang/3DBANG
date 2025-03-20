#include "GameMode/BangGameModeBase.h"
#include "PlayerController/BangPlayerController.h"

void ABangGameModeBase::StartGame_Implementation()
{/*
    if (HasAuthority())
    {
        ABangGameState* GS = GetGameState<ABangGameState>();
        if (GS)
        {
            GS->bIsGameStarted = true;
            GS->OnRep_bIsGameStarted();
        }

        AssignRoles();

        Multicast_StartGame();

        StartFirstTurn();
    }*/
}

void ABangGameModeBase::Multicast_StartGame_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("Game Started!"));

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABangPlayerController* PC = Cast<ABangPlayerController>(*It);
        if (PC)
        {
           // PC->Client_ShowGameStartUI();
        }
    }
}

void ABangGameModeBase::AssignRoles()
{
    /*if (!HasAuthority()) return; // 서버에서만 실행

    TArray<EPlayerRole> AvailableRoles;
    int32 NumPlayers = PlayerArray.Num();

    switch (NumPlayers)
    {
        case 4:
            AvailableRoles = { EPlayerRole::Sheriff, EPlayerRole::Renegade, EPlayerRole::Outlaw, EPlayerRole::Outlaw };
            break;
        case 5:
            AvailableRoles = { EPlayerRole::Sheriff, EPlayerRole::Renegade, EPlayerRole::Outlaw, EPlayerRole::Outlaw, EPlayerRole::Deputy };
            break;
        case 6:
            AvailableRoles = { EPlayerRole::Sheriff, EPlayerRole::Renegade, EPlayerRole::Outlaw, EPlayerRole::Outlaw, EPlayerRole::Outlaw, EPlayerRole::Deputy };
            break;
        case 7:
            AvailableRoles = { EPlayerRole::Sheriff, EPlayerRole::Renegade, EPlayerRole::Outlaw, EPlayerRole::Outlaw, EPlayerRole::Outlaw, EPlayerRole::Deputy, EPlayerRole::Deputy };
            break;
        default:
            UE_LOG(LogTemp, Warning, TEXT("Unsupported player count: %d"), NumPlayers);
            return;
    }

    for (int32 i = AvailableRoles.Num() - 1; i > 0; i--)
    {
        int32 j = FMath::RandRange(0, i);
        AvailableRoles.Swap(i, j);
    }

    for (int32 i = 0; i < NumPlayers; i++)
    {
        ABangPlayerState* PlayerState = Cast<ABangPlayerState>(PlayerArray[i]);
        if (PlayerState && AvailableRoles.Num() > 0)
        {
            PlayerState->SetRole(AvailableRoles.Pop());
        }
    }*/
}

void ABangGameModeBase::ShuffleRoles(TArray<EPlayerRole>& Roles)
{
    /*for (int32 i = Roles.Num() - 1; i > 0; i--)
    {
        int32 j = FMath::RandRange(0, i);
        Roles.Swap(i, j);
    }*/
}

void ABangGameModeBase::StartFirstTurn()
{
    /*if (HasAuthority())
    {
        int32 SheriffIndex = -1;

        for (int32 i = 0; i < PlayerArray.Num(); i++)
        {
            BangPlayerState* PlayerState = Cast<BangPlayerState>(PlayerArray[i]);
            if (PlayerState && PlayerState->GetRole() == EPlayerRole::Sheriff)
            {
                SheriffIndex = i;
                break;
            }
        }

        if (SheriffIndex != -1)
        {
            CurrentTurnPlayerIndex = SheriffIndex;
            StartTurn(CurrentTurnPlayerIndex);
        }
    }*/
}

void ABangGameModeBase::StartTurn(int32 PlayerIndex)
{
    /*if (HasAuthority() && PlayerIndex >= 0 && PlayerIndex < PlayerArray.Num())
    {
        BangPlayerState* PlayerState = Cast<BangPlayerState>(PlayerArray[PlayerIndex]);
        if (PlayerState)
        {
            DrawCardForPlayer(PlayerState);
            DrawCardForPlayer(PlayerState);

            Multicast_NotifyTurn(PlayerIndex);
        }
    }*/
}

void ABangGameModeBase::EndTurn()
{
    /*
    if (HasAuthority())
    {
        BangPlayerState* PlayerState = Cast<BangPlayerState>(PlayerArray[CurrentTurnPlayerIndex]);
        if (PlayerState)
        {
            int32 MaxHandSize = PlayerState->GetHealth();
            PlayerState->DiscardExcessCards(MaxHandSize);
        }

        NextTurn();
    }*/
}

void ABangGameModeBase::NextTurn()
{/*
    if (HasAuthority())
    {
        int32 NumPlayers = PlayerArray.Num();
        if (NumPlayers > 0)
        {
            CurrentTurnPlayerIndex = (CurrentTurnPlayerIndex + 1) % NumPlayers;
            StartTurn(CurrentTurnPlayerIndex);
        }
    }*/
}

void ABangGameModeBase::CheckGameEnd()
{
    if (!HasAuthority()) return;

}

/*void ABangGameModeBase::Multicast_AnnounceWinner_Implementation(const FString& WinningTeam)
{
    UE_LOG(LogTemp, Log, TEXT("Game Over! %s"), *WinningTeam);
}*/

/*
void ABangGameModeBase::HandleAttack(BangPlayerState* Attacker, BangPlayerState* Defender)
{
    if (!Attacker || !Defender) return;

    if (!Attacker->IsTargetInRange(Defender))
    {
        UE_LOG(LogTemp, Warning, TEXT("Target is out of range!"));
        return;
    }
    if (Defender->HasDodgeCard())
    {
        Defender->AskToDodge();
        return;
    }
    ApplyDamage(Defender);
}*/

/*
void ABangGameModeBase::HandlePlayerDeath(BangPlayerState* DeadPlayer)
{
    if (!DeadPlayer) return;

    DeadPlayer->SetIsDead(true);
    UE_LOG(LogTemp, Log, TEXT("%s has died!"), *DeadPlayer->GetPlayerName());

    CheckGameEnd();
}*/

/*void ABangGameModeBase::HandleAttack(ABangPlayerState* Attacker, ABangPlayerState* Defender)
{
    if (!Attacker || !Defender) return;

    UE_LOG(LogTemp, Log, TEXT("%s is attacking %s"), *Attacker->GetPlayerName(), *Defender->GetPlayerName());

    Defender->ReceiveAttack(Attacker);
}*/


ABangGameModeBase::ABangGameModeBase()
{
	DefaultPawnClass = ABangCharacter::StaticClass();
	PlayerControllerClass = ABangPlayerController::StaticClass();
}