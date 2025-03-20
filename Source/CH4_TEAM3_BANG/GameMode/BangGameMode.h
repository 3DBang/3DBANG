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

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawning")
	float Radius=500.f;

	// 원멍 : 플레이어 컨트롤러 임시저장 
	TArray<APlayerController*> PlayerControllers;

	UFUNCTION(BlueprintCallable)
	void SpawnPlayers();

	UFUNCTION(BlueprintCallable)
	APlayerStart* ChooseStartLocation() const;

	/**Test for SpawnActor*/
	UFUNCTION(BlueprintCallable)
	void SpawnPlayerBlue();

};
