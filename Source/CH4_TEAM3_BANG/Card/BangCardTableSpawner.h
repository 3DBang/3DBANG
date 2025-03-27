#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BangCardTableSpawner.generated.h"

UCLASS()
class CH4_TEAM3_BANG_API ABangCardTableSpawner : public AActor
{
	GENERATED_BODY()

public:
	ABangCardTableSpawner();

	UPROPERTY(EditAnywhere, Category = "Card")
	int32 MaxPlayerCount = 4;

	UPROPERTY(EditAnywhere, Category = "Card")
	TSubclassOf<class ABangCardActor> CardActorClass;

	UPROPERTY(EditAnywhere, Category = "Card")
	class UBangCardManager* CardManager;

	virtual void BeginPlay() override;
	void SpawnDeckCards();
	void SpawnHandCards();
private:

};
