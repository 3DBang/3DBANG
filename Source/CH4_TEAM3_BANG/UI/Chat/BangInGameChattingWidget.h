#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "BangInGameChattingWidget.generated.h"

class UEditableText;
class UScrollBox;

UCLASS()
class CH4_TEAM3_BANG_API UBangInGameChattingWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UScrollBox> ChatScrollBox;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UEditableText> ChatTextField;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton> StartButton;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton> TestButton;

	UFUNCTION(BlueprintCallable)
	void AddMessage(const FText& Message, const FSlateColor& Color);

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnTextCommittedFunction(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnStartButtonClicked();

	UFUNCTION()
	void OnTestButtonClicked();
};