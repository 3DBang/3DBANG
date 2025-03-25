#include "UI/Chat/PlayerListGameLog.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "UObject/ConstructorHelpers.h"
#include "Data/PlayerInformation.h" // ✅ 필수!
#include "PlayerState/BangPlayerState.h"

UPlayerListGameLog::UPlayerListGameLog(const FObjectInitializer& ObjectInitializer)
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

void UPlayerListGameLog::NativeConstruct()
{
	Super::NativeConstruct();
	// FObjectFinder는 여기 쓰면 안 됨 ❌
}

void UPlayerListGameLog::AddGameLog(const FString& LogText)
{
	if (!GameLogScrollBox) return;

	UTextBlock* LogEntry = NewObject<UTextBlock>(this);
	LogEntry->SetText(FText::FromString(LogText));
	LogEntry->Font.Size = 16;
	LogEntry->SetColorAndOpacity(FSlateColor(FLinearColor::White));

	GameLogScrollBox->AddChild(LogEntry);
	GameLogScrollBox->ScrollToEnd();
}

void UPlayerListGameLog::ClearPlayerList()
{
	if (PlayerListBox)
	{
		PlayerListBox->ClearChildren();
	}
}

void UPlayerListGameLog::UpdatePlayerList(const TArray<FPlayerInformation>& PlayerList)
{
	if (!PlayerListBox) return;

	PlayerListBox->ClearChildren();

	for (const FPlayerInformation& Info : PlayerList)
	{
		bool bIsAlive = Info.CurrentHealth > 0;
		AddPlayerToList(Info.PlayerName, bIsAlive);
	}
}

void UPlayerListGameLog::AddPlayerToList(const FString& PlayerName, bool bIsAlive)
{
	if (!PlayerListBox) return;

	UHorizontalBox* PlayerEntry = NewObject<UHorizontalBox>(this);

	UImage* StatusIcon = NewObject<UImage>(this);
	UTexture2D* Icon = bIsAlive ? AliveIcon : DeadIcon;
	if (Icon)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(Icon);
		Brush.ImageSize = FVector2D(32.f, 32.f);
		StatusIcon->SetBrush(Brush);
	}

	UTextBlock* NameText = NewObject<UTextBlock>(this);
	NameText->SetText(FText::FromString(PlayerName));
	NameText->Font.Size = 18;

	PlayerEntry->AddChildToHorizontalBox(StatusIcon);
	PlayerEntry->AddChildToHorizontalBox(NameText);

	PlayerListBox->AddChildToVerticalBox(PlayerEntry);
}
