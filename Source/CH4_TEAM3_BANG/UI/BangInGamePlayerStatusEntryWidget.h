#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "BangInGamePlayerStatusEntryWidget.generated.h"

UCLASS()
class CH4_TEAM3_BANG_API UBangInGamePlayerStatusEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	void NativeOnListItemObjectSet(UObject* ListItemObject) const;

	UPROPERTY(meta = (BindWidget))
	class UImage* StatusImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerIdText;

private:
	UPROPERTY()
	TObjectPtr<UTexture2D> AliveTexture;
	UPROPERTY()
	TObjectPtr<UTexture2D> DeadTexture;
};
