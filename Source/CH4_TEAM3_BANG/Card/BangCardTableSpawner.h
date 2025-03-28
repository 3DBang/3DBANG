#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Card/BangCardManager.h"
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
	void SpawnUsedCards();
	//void SpawnEquippedCards(); 패시브 카드 장착했을때의



	UPROPERTY()
	ABangCardActor* CurrentUsedCardActor = nullptr;

	void RemoveCardActor(const FSingleCard& CardToRemove);
	void RefreshEquippedCards();

private:

	UPROPERTY()
	TArray<ABangCardActor*> SpawnedCardActors; 

	FVector GetEquipSlotPositionForPlayer(uint32 PlayerUniqueID, int32 EquipCardIndex) const;

};
