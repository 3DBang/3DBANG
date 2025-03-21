#include "BangInGamePlayerListWidget.h"

#include "Components/ListView.h"
#include "Data/BangPlayerStatData.h"

void UBangInGamePlayerListWidget::UpdatePlayerList(const TArray<UBangPlayerStatData*>& PlayerStats) const
{
	PlayerListView->ClearListItems();

	UE_LOG(LogTemp, Error, TEXT("UpdatePlayerList"));

	for (UBangPlayerStatData* Data : PlayerStats)
	{
		PlayerListView->AddItem(Data);
	}
}
