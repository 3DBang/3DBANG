#include "Card/BangCardActor.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Card/BaseCard/BangCardBase.h"
#include "PlayerState/BangPlayerState.h"
#include "PlayerController/BangPlayerController.h"

ABangCardActor::ABangCardActor()
{
    PrimaryActorTick.bCanEverTick = false;

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    CardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardMesh"));
    CardMesh->SetupAttachment(Root);

    CardFrontWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CardFrontWidget"));
    CardFrontWidget->SetupAttachment(CardMesh);
    CardFrontWidget->SetWidgetSpace(EWidgetSpace::World);
    CardFrontWidget->SetDrawSize(FVector2D(512, 512));
    CardFrontWidget->SetRelativeLocation(FVector(0.f, 0.f, 0.1f));
    CardFrontWidget->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

    CardBackWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CardBackWidget"));
    CardBackWidget->SetupAttachment(CardMesh);
    CardBackWidget->SetWidgetSpace(EWidgetSpace::World);
    CardBackWidget->SetDrawSize(FVector2D(512, 512));
    CardBackWidget->SetRelativeLocation(FVector(0.f, 0.f, -0.1f));
    CardBackWidget->SetRelativeRotation(FRotator(180.f, 0.f, 0.f));

    bReplicates = true;
    SetReplicateMovement(true);
}

void ABangCardActor::BeginPlay()
{
    Super::BeginPlay();

    CardFrontWidget->SetVisibility(true);
    CardBackWidget->SetVisibility(false);
    bIsFrontVisible = true;

    GetWorldTimerManager().SetTimerForNextTick(this, &ABangCardActor::UpdateWidgetContent);
}

void ABangCardActor::SetCard(const FSingleCard& InCard, bool bForceFront)
{
    CardData = InCard;

    bIsFrontVisible = bForceFront;

    CardFrontWidget->SetVisibility(bIsFrontVisible);
    CardBackWidget->SetVisibility(!bIsFrontVisible);

    UpdateWidgetContent();

}

// 수정필요
void ABangCardActor::Multicast_SetCard_Implementation(const FSingleCard& InCard, bool /*bForceFront*/)
{
    CardData = InCard;

    bool bIsMyCard = false;

    if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
    {
        if (APlayerController* PC = LocalPlayer->GetPlayerController(GetWorld()))
        {
            if (const ABangPlayerController* BangPC = Cast<ABangPlayerController>(PC))
            {
                if (ABangPlayerState* PS = BangPC->GetPlayerState<ABangPlayerState>())
                {
                    FCardCollection MyCards;
                    PS->GetCard(PS->PlayerUniqueID, MyCards);

                    for (const FSingleCard& MyCard : MyCards.CardList)
                    {
                        if (MyCard.Card &&
                            InCard.Card &&
                            MyCard.Card->SymbolType == InCard.Card->SymbolType &&
                            MyCard.Card->SymbolNumber == InCard.Card->SymbolNumber)
                        {
                            bIsMyCard = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    SetCard(InCard, bIsMyCard);
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

