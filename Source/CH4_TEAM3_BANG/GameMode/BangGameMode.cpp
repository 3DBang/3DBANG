#include "BangGameMode.h"

#include "Card/BangCardManager.h"
#include "Card/CharacterCard/BangCharacterCard.h"
#include "Card/JobCard/BangJobCard.h"

#include "BlueprintDataDefinitions.h"
#include "AdvancedSteamFriendsLibrary.h"

#include "GameState/BangGameState.h"
#include "PlayerState/BangPlayerState.h"
#include "PlayerController/BangPlayerController.h"
#include "../BangCharacter/BangCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
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
	
	CurrentPlayerTurnState = EPlayerTurnState::DrawCard;
}

void ABangGameMode::AddPlayer(const FBPUniqueNetId& PlayerNetID)
{
	if (CurrentGameState == EGameState::GamePlaying) return;
	
	//FString PlayerID = UAdvancedSteamFriendsLibrary::GetSteamPersonaName(PlayerNetID);
	FPlayerInformation PlayerInfo;
	PlayerInfo.PlayerNetID = PlayerNetID;
	//PlayerInfo.PlayerName = PlayerID;
	Players.Players.Add(PlayerInfo);
}

void ABangGameMode::ArrangeSeats()
{
	ShuffleSeats(Players);
	// 플레이어 스테이트에 Players 전달
}

void ABangGameMode::ShuffleSeats(FPlayerCollection& ToShufflePlayers) const
{
	if (CurrentGameState == EGameState::GamePlaying || ToShufflePlayers.Players.Num() < 4 || ToShufflePlayers.Players.Num() > 7) return;

	const int32 LastIndex = ToShufflePlayers.Players.Num() - 1;
	for (int32 i = LastIndex; i > 0; --i)
	{
		const int32 RandomIndex = FMath::RandRange(0, i);
		ToShufflePlayers.Players.Swap(i, RandomIndex);
	}
}

void ABangGameMode::StartGame()
{
	if (CurrentGameState == EGameState::GamePlaying || !CardManager || Players.Players.Num() < 4 || Players.Players.Num() > 7) return;

	CurrentGameState = EGameState::GamePlaying;

	// 카드 매니저에 게임 시작 알림
	CardManager->PlayBeginByRole();
	
	// 직업 선택
	FCardCollection JobCards;
	CardManager->GetJobByPlayer(Players.Players.Num(), JobCards);
	FPlayerInformation PlayerInfo;
	
	// 케릭터 카드 분배 & 보안관 먼저 시작
	FCardCollection CharacterCards;
	for (int16 i = 0; i < Players.Players.Num(); i++)
	{
		Players.Players[i].JobCard = JobCards.CardList[i];
		Players.Players[i].CharacterCard = CardManager->GetCharacterCard();
		
		if (const TObjectPtr<UBangJobCard> JobCard = Cast<UBangJobCard>(JobCards.CardList[i]))
		{
			if (JobCard->JobType == EJobType::Officer)
			{
				CurrentPlayerTurn = Players.Players[i].PlayerName;
				PlayerIndex = i;
			}
		}
	}

	AdvanceGameTurn();
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



void ABangGameMode::AdvanceGameTurn()
{
	if (CurrentGameState == EGameState::GameOver || !CardManager) return;

	if (CurrentPlayerTurnState == EPlayerTurnState::DrawCard)
	{
		if (const TObjectPtr<UBangCharacterCard> CharacterCard = Cast<UBangCharacterCard>(Players.Players[PlayerIndex].CharacterCard))
		{
			switch (CharacterCard->CharacterType)
			{
			case ECharacterType::PedroRamirez:
				{
					FCardCollection DrawCards;
					CardManager->HandCards(3, DrawCards);
					// 플레이어 스테이트 한테 넘겨줌
					break;
				}
			default: break;
			}
		}
		// 플레이어 한테 응답 받고 아래 로직 실행
		CurrentPlayerTurnState = EPlayerTurnState::UseCard;
		AdvanceGameTurn();
	}
	else if (CurrentPlayerTurnState == EPlayerTurnState::UseCard)
	{
		

		// 플레이어 한테 응답 받고 아래 로직 실행
		CurrentPlayerTurnState = EPlayerTurnState::LooseCard;
		AdvanceGameTurn();
	}
	else if (CurrentPlayerTurnState == EPlayerTurnState::LooseCard)
	{
		
	}
}

void ABangGameMode::AdvancePlayerTurn()
{
	if (CurrentPlayerTurnState == EPlayerTurnState::LooseCard)
	{
		PlayerIndex++;
		PlayerIndex = PlayerIndex % Players.Players.Num();
		CurrentPlayerTurn = Players.Players[PlayerIndex].PlayerName;
	}
}

