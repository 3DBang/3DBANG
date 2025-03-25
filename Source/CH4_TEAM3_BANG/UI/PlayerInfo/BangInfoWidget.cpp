#include "UI/PlayerInfo/BangInfoWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UBangInfoWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (CloseButton)
    {
        CloseButton->OnClicked.AddDynamic(this, &UBangInfoWidget::CloseWidget);
    }
    UpdateRange(Range);
    UpdateCurrentHealth(CurrentHealth);
}
void UBangInfoWidget::UpdateRange(int32 NewRange)
{
    Range = NewRange;
    if (RangeText)
    {
        RangeText->SetText(FText::AsNumber(Range));
    }
}

void UBangInfoWidget::UpdateCurrentHealth(int32 NewHealth)
{
    CurrentHealth = NewHealth;
    if (HealthText)
    {
        HealthText->SetText(FText::AsNumber(CurrentHealth));
    }
}

void UBangInfoWidget::CloseWidget()
{
    RemoveFromParent();
}