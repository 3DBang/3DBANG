#include "BangGameInstance.h"

#include "Card/BangCardManager.h"
#include "Kismet/GameplayStatics.h"


void UBangGameInstance::Init()
{
	Super::Init();

	if (!CardManager)
	{
		CardManager = NewObject<UBangCardManager>(this, UBangCardManager::StaticClass());
		if (CardManager)
		{
			UE_LOG(LogTemp, Log, TEXT("CardManager initialized successfully in GameInstance."));
		}
	}
}

void UBangGameInstance::GetCardManager(FCardManagerInstance& OutCardManager)
{
	if (!CardManager) return;
	OutCardManager.CardManager = CardManager;
}