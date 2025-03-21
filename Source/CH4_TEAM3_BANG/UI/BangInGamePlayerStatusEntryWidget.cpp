#include "BangInGamePlayerStatusEntryWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/BangPlayerStatData.h"

void UBangInGamePlayerStatusEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject) const
{
	if (const UBangPlayerStatData* Data = Cast<UBangPlayerStatData>(ListItemObject))
	{
		PlayerIdText->SetText(FText::FromString(Data->PlayerId));

		if (Data->bIsAlive)
		{
			StatusImage->SetBrushFromTexture(AliveTexture);  // 사전 로딩 필요
		}
		else
		{
			StatusImage->SetBrushFromTexture(DeadTexture);  // 사전 로딩 필요
		}
	}
}
