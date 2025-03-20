#include "CH4_TEAM3_BANGGameMode.h"
#include "UObject/ConstructorHelpers.h"


ACH4_TEAM3_BANGGameMode::ACH4_TEAM3_BANGGameMode()
{
   // static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BANG/BongBP/MyCH4_TEAM3_BANGCharacter"));
    if (PlayerPawnBPClass.Class != NULL)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }

    PlayerControllerClass = ABangPlayerController::StaticClass();

}

void ACH4_TEAM3_BANGGameMode::StartGame_Implementation()
{/*
    if (HasAuthority())
    {
        ACH4_TEAM3_BANGGameState* GS = GetGameState<ACH4_TEAM3_BANGGameState>();
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

void ACH4_TEAM3_BANGGameMode::Multicast_StartGame_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("Game Started!"));

    /*for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ACH4_TEAM3_PlayerController* PC = Cast<ACH4_TEAM3_PlayerController>(*It);
        if (PC)
        {
            PC->Client_ShowGameStartUI();
        }
    }*/
}

void ACH4_TEAM3_BANGGameMode::AssignRoles()
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
        ACH4_TEAM3_PlayerState* PlayerState = Cast<ACH4_TEAM3_PlayerState>(PlayerArray[i]);
        if (PlayerState && AvailableRoles.Num() > 0)
        {
            PlayerState->SetRole(AvailableRoles.Pop());
        }
    }*/
}

void ACH4_TEAM3_BANGGameMode::ShuffleRoles(TArray<EPlayerRole>& Roles)
{
    /*for (int32 i = Roles.Num() - 1; i > 0; i--)
    {
        int32 j = FMath::RandRange(0, i);
        Roles.Swap(i, j);
    }*/
}

void ACH4_TEAM3_BANGGameMode::StartFirstTurn()
{
    /*if (HasAuthority())
    {
        int32 SheriffIndex = -1;

        for (int32 i = 0; i < PlayerArray.Num(); i++)
        {
            ACH4_TEAM3_PlayerState* PlayerState = Cast<ACH4_TEAM3_PlayerState>(PlayerArray[i]);
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

void ACH4_TEAM3_BANGGameMode::StartTurn(int32 PlayerIndex)
{
    /*if (HasAuthority() && PlayerIndex >= 0 && PlayerIndex < PlayerArray.Num())
    {
        ACH4_TEAM3_PlayerState* PlayerState = Cast<ACH4_TEAM3_PlayerState>(PlayerArray[PlayerIndex]);
        if (PlayerState)
        {
            DrawCardForPlayer(PlayerState);
            DrawCardForPlayer(PlayerState);

            Multicast_NotifyTurn(PlayerIndex);
        }
    }*/
}

void ACH4_TEAM3_BANGGameMode::EndTurn()
{
    /*
    if (HasAuthority())
    {
        ACH4_TEAM3_PlayerState* PlayerState = Cast<ACH4_TEAM3_PlayerState>(PlayerArray[CurrentTurnPlayerIndex]);
        if (PlayerState)
        {
            int32 MaxHandSize = PlayerState->GetHealth();
            PlayerState->DiscardExcessCards(MaxHandSize);
        }

        NextTurn();
    }*/
}

void ACH4_TEAM3_BANGGameMode::NextTurn()
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

void ACH4_TEAM3_BANGGameMode::CheckGameEnd()
{
    if (!HasAuthority()) return;

}

/*void ACH4_TEAM3_BANGGameMode::Multicast_AnnounceWinner_Implementation(const FString& WinningTeam)
{
    UE_LOG(LogTemp, Log, TEXT("Game Over! %s"), *WinningTeam);
}*/

/*
void ACH4_TEAM3_BANGGameMode::HandleAttack(ACH4_TEAM3_PlayerState* Attacker, ACH4_TEAM3_PlayerState* Defender)
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
void ACH4_TEAM3_BANGGameMode::HandlePlayerDeath(ACH4_TEAM3_PlayerState* DeadPlayer)
{
    if (!DeadPlayer) return;

    DeadPlayer->SetIsDead(true);
    UE_LOG(LogTemp, Log, TEXT("%s has died!"), *DeadPlayer->GetPlayerName());

    CheckGameEnd();
}*/
