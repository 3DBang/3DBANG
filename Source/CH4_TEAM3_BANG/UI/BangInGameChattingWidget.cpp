#include "BangInGameChattingWidget.h"

#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "PlayerController/BangPlayerController.h"

void UBangInGameChattingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Error, TEXT("ABangPlayerController BeginPlay"));

	if (IsValid(ChatTextField))
	{
		ChatTextField->OnTextCommitted.AddDynamic(this, &UBangInGameChattingWidget::OnTextCommittedFunction);
	}

	if (IsValid(StartButton))
	{
		StartButton->OnClicked.AddDynamic(this, &UBangInGameChattingWidget::OnStartButtonClicked);
	}
}

void UBangInGameChattingWidget::AddMessage(const FText& Message, const FSlateColor& Color)
{
	if (!IsValid(ChatScrollBox)) return;

	if (const TObjectPtr<UTextBlock> NewMessage = NewObject<UTextBlock>(this))
	{
		NewMessage->SetText(Message);
		NewMessage->Font.Size = 20;
		NewMessage->SetColorAndOpacity(Color);

		ChatScrollBox->AddChild(NewMessage);
		ChatScrollBox->ScrollToEnd();
	}
}

void UBangInGameChattingWidget::OnStartButtonClicked()
{
	if (const TObjectPtr<ABangPlayerController> OwningPlayerController = Cast<ABangPlayerController>(GetOwningPlayer()))
	{
		OwningPlayerController->StartButtonCLicked();
	}
}

void UBangInGameChattingWidget::OnTextCommittedFunction(const FText& Text, const ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (Text.IsEmpty()) return;

		const ABangPlayerController* Controller = Cast<ABangPlayerController>(GetOwningPlayer());
		if (!Controller)
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerController is NULL!"));
			return;
		}
		
		//Controller->SendMessageToServer(Text.ToString());
		ChatTextField->SetText(FText::FromString(""));
	}
}