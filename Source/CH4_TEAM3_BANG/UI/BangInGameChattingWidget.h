#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
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
	UBangInGameChattingWidget(const FObjectInitializer& ObjectInitializer); // 

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ChatScrollBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableText> ChatTextField;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> TestButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> PlayerListBox;

	UFUNCTION(BlueprintCallable)
	void AddMessage(const FText& Message, const FSlateColor& Color);

	UFUNCTION(BlueprintCallable)
	void AddPlayerToList(const FString& PlayerName, bool bIsAlive);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icons")
	UTexture2D* AliveIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icons")
	UTexture2D* DeadIcon;

	UFUNCTION(BlueprintCallable)
	void ClearPlayerList();
	
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerList(const TArray<FPlayerInformation>& PlayerList);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void OnStartButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnTestButtonClicked();

private:
	UFUNCTION()
	void OnTextCommittedFunction(const FText& Text, const ETextCommit::Type CommitMethod);

	
};
