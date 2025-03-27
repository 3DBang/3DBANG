#include "Card/BangCardActor.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Net/UnrealNetwork.h"

#include "Card/BaseCard/BangCardBase.h"

ABangCardActor::ABangCardActor()
{
    PrimaryActorTick.bCanEverTick = false;

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    CardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardMesh"));
    CardMesh->SetupAttachment(Root);

    // 앞면 위젯
    CardFrontWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CardFrontWidget"));
    CardFrontWidget->SetupAttachment(CardMesh);
    CardFrontWidget->SetWidgetSpace(EWidgetSpace::World);
    CardFrontWidget->SetDrawSize(FVector2D(512, 512));
    CardFrontWidget->SetRelativeLocation(FVector(0.f, 0.f, 0.1f));
    CardFrontWidget->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

    // 뒷면 위젯
    CardBackWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CardBackWidget"));
    CardBackWidget->SetupAttachment(CardMesh);
    CardBackWidget->SetWidgetSpace(EWidgetSpace::World);
    CardBackWidget->SetDrawSize(FVector2D(512, 512));
    CardBackWidget->SetRelativeLocation(FVector(0.f, 0.f, -0.1f));
    CardBackWidget->SetRelativeRotation(FRotator(180.f, 0.f, 0.f)); 

    bReplicates = true;
    SetReplicateMovement(true); // 카드 위치 복사
}

void ABangCardActor::BeginPlay()
{
	Super::BeginPlay();

	// 기본적으로 앞면 표시
	CardFrontWidget->SetVisibility(true);
	CardBackWidget->SetVisibility(false);
	bIsFrontVisible = true;

	// 위젯 초기화 이후 호출
	GetWorldTimerManager().SetTimerForNextTick(this, &ABangCardActor::UpdateWidgetContent);
}

void ABangCardActor::SetCard(const FSingleCard& InCard, bool bShowFront)
{
    CardData = InCard;
    bIsFrontVisible = bShowFront;

    CardFrontWidget->SetVisibility(bShowFront);
    CardBackWidget->SetVisibility(!bShowFront);

    // 새 카드 정보 들어왔을 때 즉시 이미지 갱신 시도
    UpdateWidgetContent();
}
void ABangCardActor::UpdateWidgetContent()
{
    if (CardData.Card && CardData.Card->CardIcon && CardFrontWidget)
    {
        UUserWidget* Widget = CardFrontWidget->GetUserWidgetObject();
        if (Widget)
        {
            UImage* Image = Cast<UImage>(Widget->GetWidgetFromName(TEXT("CardIconImage")));
            if (Image)
            {
                FSlateBrush NewBrush;
                NewBrush.SetResourceObject(CardData.Card->CardIcon);
                NewBrush.ImageSize = FVector2D(512, 512);
                Image->SetBrush(NewBrush);
                UE_LOG(LogTemp, Warning, TEXT("이미지 설정 완료됨!"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("CardIconImage 찾기 실패!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("위젯 객체 없음!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CardData/CardIcon/CardFrontWidget 없음!"));
    }
}

void ABangCardActor::Multicast_SetCard_Implementation(const FSingleCard& InCard, bool bShowFront)
{
    SetCard(InCard, bShowFront);
}