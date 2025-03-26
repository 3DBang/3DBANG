// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/PlayerInformation.h"
#include "BangInfoWidget.generated.h"

class UScrollBox;
struct FSingleCard;
enum class ECharacterType : uint8;
class UCard;
class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSelectedDelegate, uint32, PlayerUniqueID);

UCLASS()
class CH4_TEAM3_BANG_API UBangInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "PlayerInfo")
	UCard* CharacterCard;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "PlayerInfo")
	UTextBlock* PlayerNameText; // 플레이어 이름 텍스트 블록

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "PlayerInfo")
	UTextBlock* CardCountText; // 보유카드 수 텍스트 블록

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "PlayerInfo")
	UTextBlock* TurnStatusText; // 턴 상태 텍스트 블록

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "CardList")
	UScrollBox* ScrollBox;		//장착된 카드 리스트

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "PlayerInfo")
	UButton* UseCardButton;		// 이 플레이어에게 사용 하는 버튼

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Card")
	TSubclassOf<UUserWidget> CardWidgetClass;
	
	FOnPlayerSelectedDelegate PlayerSelectedDelegate;

	uint32 TargetPlayerUniqueID;
	
protected:
    virtual void NativeConstruct() override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
public:
	
	UFUNCTION()
	void OnUseCardButtonClicked();
	
	UFUNCTION()
	void ShowPlayerInfo(uint32 TargetPlayerUniqueID);

	FPlayerInformation* GetTargetPlayerInfo(uint32 TargetPlayerUniqueID);
	ABangPlayerState* GetBangPlayerState() const;
};
