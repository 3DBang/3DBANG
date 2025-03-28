#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Card/BangCardManager.h"
#include "BangCardActor.generated.h"

UCLASS()
class CH4_TEAM3_BANG_API ABangCardActor : public AActor
{
	GENERATED_BODY()

public:
	ABangCardActor();
	void UpdateWidgetContent();

protected:
	virtual void BeginPlay() override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* CardMesh;

	/** 카드 앞면 위젯 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Card|Widget", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* CardFrontWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Card|Widget", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* CardBackWidget;

	/** 카드 데이터 */
	UPROPERTY(BlueprintReadOnly)
	FSingleCard CardData;

	/** 카드 설정 (데이터 + 앞면 표시 여부) */
	void SetCard(const FSingleCard& InCard, bool bShowFront);

	/** 카드 앞/뒷면 뒤집기 */
	void FlipCard();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetCard(const FSingleCard& InCard, bool bShowFront);

	UPROPERTY()
	uint32 OwnerUniqueID = -1;


private:
	bool bIsFrontVisible = true;
};
