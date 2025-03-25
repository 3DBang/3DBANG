#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerListGameLog.generated.h"

class UScrollBox;
class UVerticalBox;
class UTextBlock;
class UImage;

UCLASS()
class CH4_TEAM3_BANG_API UPlayerListGameLog : public UUserWidget
{
	GENERATED_BODY()

public:
	UPlayerListGameLog(const FObjectInitializer& ObjectInitializer);  

	// 플레이어 리스트 영역
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> PlayerListBox;

	// 게임 로그 영역
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> GameLogScrollBox;

	// 플레이어 상태 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icons")
	UTexture2D* AliveIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icons")
	UTexture2D* DeadIcon;

	// 메세지 추가
	UFUNCTION(BlueprintCallable)
	void AddGameLog(const FString& LogText);

	// 플레이어 목록 업데이트
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerList(const TArray<FPlayerInformation>& PlayerList);

	UFUNCTION(BlueprintCallable)
	void ClearPlayerList();

protected:
	virtual void NativeConstruct() override;

	// 플레이어 항목 추가
	void AddPlayerToList(const FString& PlayerName, bool bIsAlive);
};


