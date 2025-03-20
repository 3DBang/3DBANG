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
