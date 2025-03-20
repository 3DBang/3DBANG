#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BangGameMode.generated.h"

UCLASS()
class CH4_TEAM3_BANG_API ABangGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABangGameMode();
	virtual void BeginPlay() override;

	void PrintPlayerStats();

	FTimerHandle TimerHandle;

	virtual void PostLogin(APlayerController* NewPlayer) override;
};
