#include "BangGameInstance.h"

#include "Card/BangCardManager.h"

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

void UBangGameInstance::GetCardManager(FCardManagerInstance& OutCardManager) const
{
	if (!CardManager) return;
	OutCardManager.CardManager = CardManager;
}
