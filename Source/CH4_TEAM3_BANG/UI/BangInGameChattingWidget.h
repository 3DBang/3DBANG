#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BangInGameChattingWidget.generated.h"

class UEditableText;
class UScrollBox;

UCLASS()
class CH4_TEAM3_BANG_API UBangInGameChattingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UScrollBox* ChatScrollBox;

	UPROPERTY(meta = (BindWidget))
	UEditableText* ChatTextField;

	UFUNCTION(BlueprintCallable)
	void AddMessage(const FText& Message, const FSlateColor& Color);
	
protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnTextCommittedFunction(const FText& Text, const ETextCommit::Type CommitMethod);
};
