#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BangInGamePlayerListWidget.generated.h"

class UBangPlayerStatData;

UCLASS()
class CH4_TEAM3_BANG_API UBangInGamePlayerListWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UListView* PlayerListView;

	void UpdatePlayerList(const TArray<UBangPlayerStatData*>& PlayerStats) const;
};
