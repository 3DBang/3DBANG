#include "BangInGameChattingWidget.h"

#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "PlayerController/BangPlayerController.h"
#include "PlayerState/BangPlayerState.h"
#include "Components/Image.h"
#include "UObject/ConstructorHelpers.h"

UBangInGameChattingWidget::UBangInGameChattingWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> AliveTexObj(TEXT("/Game/BANG/Cards/Alive"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> DeadTexObj(TEXT("/Game/BANG/Cards/dead"));

	if (AliveTexObj.Succeeded())
	{
		AliveIcon = AliveTexObj.Object;
	}

	if (DeadTexObj.Succeeded())
	{
		DeadIcon = DeadTexObj.Object;
	}
}
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

	if (IsValid(TestButton))
	{
		TestButton->OnClicked.AddDynamic(this, &UBangInGameChattingWidget::OnTestButtonClicked);
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

void UBangInGameChattingWidget::OnTestButtonClicked()
{
	if (const TObjectPtr<ABangPlayerController> OwningPlayerController = Cast<ABangPlayerController>(GetOwningPlayer()))
	{
		OwningPlayerController->TestButtonCLicked();
	}

}

void UBangInGameChattingWidget::OnTextCommittedFunction(const FText& Text, const ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (Text.IsEmpty()) return;

		ABangPlayerController* BangPlayerController = Cast<ABangPlayerController>(GetOwningPlayer());
		if (!BangPlayerController)
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerController is NULL!"));
			return;
		}

		BangPlayerController->SendMessageToServer(Text.ToString());
		ChatTextField->SetText(FText::FromString(""));
	}
}

void UBangInGameChattingWidget::AddPlayerToList(const FString& PlayerName, bool bIsAlive)
{
	if (!IsValid(PlayerListBox)) return;

	UHorizontalBox* PlayerEntry = NewObject<UHorizontalBox>(this);

	UImage* StatusImage = NewObject<UImage>(this);
	UTexture2D* IconToUse = bIsAlive ? AliveIcon : DeadIcon;

	if (IconToUse)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(IconToUse);
		Brush.ImageSize = FVector2D(50, 50.f); 
		StatusImage->SetBrush(Brush);
	}

	UTextBlock* NameText = NewObject<UTextBlock>(this);
	NameText->SetText(FText::FromString(PlayerName));
	NameText->SetFont(FSlateFontInfo(FCoreStyle::GetDefaultFont(), 35));

	PlayerEntry->AddChildToHorizontalBox(StatusImage);
	PlayerEntry->AddChildToHorizontalBox(NameText);
	PlayerListBox->AddChildToVerticalBox(PlayerEntry);
}


void UBangInGameChattingWidget::ClearPlayerList()
{
	if (!IsValid(PlayerListBox)) return;
	PlayerListBox->ClearChildren();
}

void UBangInGameChattingWidget::UpdatePlayerList(const TArray<FPlayerInformation>& PlayerList)
{
	PlayerListBox->ClearChildren();
	UE_LOG(LogTemp, Warning, TEXT(" UpdatePlayerList 호출됨 - 총 %d명"), PlayerList.Num());

	for (const FPlayerInformation& Info : PlayerList)
	{
		FString DisplayText = FString::Printf(TEXT(" %s"), *Info.PlayerName);
		bool bIsAlive = Info.CurrentHealth > 0; 
		AddPlayerToList(DisplayText, bIsAlive);  
	}
}

void UBangInGameChattingWidget::AddGameLog(const FString& LogText)
{
	if (!IsValid(GameLogScrollBox)) return;

	UTextBlock* LogEntry = NewObject<UTextBlock>(this);
	LogEntry->SetText(FText::FromString(LogText));
	LogEntry->Font.Size = 18;
	LogEntry->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));

	GameLogScrollBox->AddChild(LogEntry);
	GameLogScrollBox->ScrollToEnd();
}