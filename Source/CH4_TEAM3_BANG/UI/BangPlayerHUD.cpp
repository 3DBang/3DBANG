#include "BangPlayerHUD.h"

#include "Blueprint/UserWidget.h"
#include "Card/RobberyChoiceWidget.h"
#include "Card/CardList.h"
#include "Card/TableCard.h"
#include "Chat/BangInGameChattingWidget.h"
#include "UI/Chat/PlayerListGameLog.h"
#include "PlayerController/BangPlayerController.h"
#include "PlayerInfo/BangInfoWidget.h"

ABangPlayerHUD::ABangPlayerHUD()
{
	WidgetOffset = FVector2D(10.0f, 10.0f);
}

void ABangPlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	APlayerController* PC = GetOwningPlayerController();

	if (ChattingWidgetClass)
	{
		ChattingWidgetInstance = CreateWidget<UBangInGameChattingWidget>(World, ChattingWidgetClass);
		if (ChattingWidgetInstance)
		{
			ChattingWidgetInstance->AddToViewport();
		}
	}

	if (CardListWidgetClass)
	{
		CardListWidgetInstance = CreateWidget<UCardList>(World, CardListWidgetClass);
		if (CardListWidgetInstance)
		{
			CardListWidgetInstance->AddToViewport();
		}
	}

	if (PlayerListGameLog)
	{
		PlayerListGameLogInstance = CreateWidget<UPlayerListGameLog>(World, PlayerListGameLog);
		if (PlayerListGameLogInstance)
		{
			PlayerListGameLogInstance->AddToViewport();
		}
	}

	if (PlayerInfoWidgetClass)
	{
		PlayerInfoWidgetInstance = CreateWidget<UBangInfoWidget>(World, PlayerInfoWidgetClass);
		if (PlayerInfoWidgetInstance)
		{
			PlayerInfoWidgetInstance->AddToViewport();
		}
	}

	//시작할땐 숨겨두기 
	if (PlayerInfoWidgetInstance)
	{
		PlayerInfoWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	}

	if (ABangPlayerController* BangPC = Cast<ABangPlayerController>(PC))
	{
		BangPC->NotifyHUDLoaded();
	}
}

void ABangPlayerHUD::ShowDrawCardUI(const FCardCollection& Cards)
{
	UE_LOG(LogTemp, Warning, TEXT("[ABangPlayerHUD::ShowDrawCardUI] 카드 수: %d"), Cards.CardList.Num());

	if (!TableCardWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerHUD::ShowDrawCardUI] TableCardWidgetClass is NULL"));
		return;
	}

	if (!TableCardWidgetInstance)
	{
		TableCardWidgetInstance = CreateWidget<UTableCard>(GetWorld(), TableCardWidgetClass);
		TableCardWidgetInstance->AddToViewport();
	}

	if (TableCardWidgetInstance)
	{
		TableCardWidgetInstance->InitializeCardList(Cards);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerHUD::ShowDrawCardUI] TableCardWidgetInstance is NULL"));
		return;
	}
}

void ABangPlayerHUD::ShowRobberyChoiceCardUI(const FCardCollection& FrontCards, const FCardCollection& BackCards,
                                             const ECardSelectPurpose CardSelectPurpose)
{
	UE_LOG(LogTemp, Warning, TEXT("[ABangPlayerHUD::ShowRobberyChoiceCardUI] 앞면 카드 수, 뒷면 카드 수: %d, %d"),
	       FrontCards.CardList.Num(), BackCards.CardList.Num());

	if (!RobberyChoiceWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerHUD::ShowRobberyChoiceCardUI] RobberyChoiceWidgetClass is NULL"));
		return;
	}

	if (!RobberyChoiceWidgetInstance)
	{
		RobberyChoiceWidgetInstance = CreateWidget<URobberyChoiceWidget>(GetWorld(), RobberyChoiceWidgetClass);
		RobberyChoiceWidgetInstance->AddToViewport();
	}

	if (RobberyChoiceWidgetInstance)
	{
		RobberyChoiceWidgetInstance->InitializeRobberyCardList(FrontCards, BackCards, CardSelectPurpose);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerHUD::ShowRobberyChoiceCardUI] RobberyChoiceWidgetInstance is NULL"));
		return;
	}
}

void ABangPlayerHUD::HideDrawCardUI()
{
	if (!TableCardWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerHUD::HideDrawCardUI] 없음"));
		return;
	}
	TableCardWidgetInstance->RemoveFromParent();
}

void ABangPlayerHUD::HideRobberyChoiceCardUI()
{
	if (!RobberyChoiceWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerHUD::HideRobberyChoiceCardUI] 없음"));
		return;
	}
	RobberyChoiceWidgetInstance->RemoveFromParent();
}

void ABangPlayerHUD::SetupTurnCardSelection(ECardSelectPurpose Purpose, FText ButtonText, int32 NumCardsToSelect)
{
	if (!CardListWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerHUD::SetupTurnCardSelection] : CardListWidgetInstance is null"));
		return;
	}

	// 버튼 텍스트 설정
	CardListWidgetInstance->UseInputButtonText->SetText(ButtonText);
	//이넘과 선택할 카드 개수
	CardListWidgetInstance->CurrentCardSelectPurpose = Purpose;
	CardListWidgetInstance->CardsToSelectCount = NumCardsToSelect;

	CardListWidgetInstance->UseInputButton->SetVisibility(ESlateVisibility::Visible);
	UE_LOG(LogTemp, Error, TEXT("[ABangPlayerHUD::SetupTurnCardSelection] : 카드 개수 설정 %d"), NumCardsToSelect);
	if (Purpose == ECardSelectPurpose::UseCard)
	{
		CardListWidgetInstance->TurnEndButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CardListWidgetInstance->TurnEndButton->SetVisibility(ESlateVisibility::Hidden);
	}

	UE_LOG(LogTemp, Log, TEXT("SetupTurnCardSelection called for Purpose: %s, ButtonText: %s, CardsToSelectCount: %d"),
	       *UEnum::GetValueAsString(Purpose), *ButtonText.ToString(), NumCardsToSelect);
}

void ABangPlayerHUD::ShowBangInfoWidget(uint32 TargetPlayerUniqueID, bool bShowButtonWidget)
{
	if (!PlayerInfoWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerHUD::ShowBangInfoWidget] : PlayerInfoWidgetInstance is null"));
		return;
	}

	if (TargetPlayerUniqueID == 0)
	{
		return;
	}

	// 마우스 커서 위치 가져오기
	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerHUD::ShowBangInfoWidget] : PlayerController is null"));
		return;
	}
	float MouseX, MouseY;
	if (!PC->GetMousePosition(MouseX, MouseY))
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerHUD::ShowBangInfoWidget] : Failed to get mouse position"));
		return;
	}

	// 화면 사이즈 가져오기
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerHUD::ShowBangInfoWidget] : GameViewport is null"));
		return; // GameViewport가 없으면 경계 처리 불가
	}

	FVector2D WidgetSize = PlayerInfoWidgetInstance->GetDesiredSize();


	// 위젯을 보이게 설정
	PlayerInfoWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	// UBangInfoWidget의 ShowPlayerInfo 함수를 호출하여 정보 업데이트
	PlayerInfoWidgetInstance->ShowPlayerInfo(TargetPlayerUniqueID);

	FVector2D WidgetPosition = FVector2D(MouseX + WidgetOffset.X, MouseY + WidgetOffset.Y);

	// 화면 경계 검사 및 조정
	float WidgetRightEdgeX = WidgetPosition.X + WidgetSize.X;
	float WidgetBottomEdgeY = WidgetPosition.Y + WidgetSize.Y;

	// 오른쪽 경계 검사
	if (WidgetRightEdgeX > ViewportSize.X)
	{
		WidgetPosition.X = ViewportSize.X - WidgetSize.X - WidgetOffset.X;
		if (WidgetPosition.X < 0) WidgetPosition.X = 0; // 왼쪽 경계에 너무 가까워지는 경우 방지
	}

	// 아래쪽 경계 검사
	if (WidgetBottomEdgeY > ViewportSize.Y)
	{
		WidgetPosition.Y = ViewportSize.Y - WidgetSize.Y - WidgetOffset.Y;
		if (WidgetPosition.Y < 0) WidgetPosition.Y = 0; // 위쪽 경계에 너무 가까워지는 경우 방지
	}

	// 위젯 위치 설정 (조정된 위치)
	PlayerInfoWidgetInstance->SetPositionInViewport(WidgetPosition, true);

	if (bShowButtonWidget) // bShowWidget가 true이면 위젯을 표시
	{
		PlayerInfoWidgetInstance->UseCardButton->SetVisibility(ESlateVisibility::Visible);
	}
	else // bShowWidget가 false이면 위젯을 숨김
	{
		PlayerInfoWidgetInstance->UseCardButton->SetVisibility(ESlateVisibility::Hidden);
	}
}
