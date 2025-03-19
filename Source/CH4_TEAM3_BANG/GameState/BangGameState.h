#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BangGameState.generated.h"

UCLASS()
class CH4_TEAM3_BANG_API ABangGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
};
