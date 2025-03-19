#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BangCardBase.generated.h"

UENUM(BlueprintType)
enum class ECardType : uint8
{
	JobCard UMETA(DisplayName = "JobCard"),
	ActiveCard UMETA(DisplayName = "ActiveCard"),
	PassiveCard UMETA(DisplayName = "PassiveCard"),
	CharacterCard UMETA(DisplayName = "CharacterCard")
};

UCLASS(Blueprintable)
class CH4_TEAM3_BANG_API UBangCardBase : public UObject
{
	GENERATED_BODY()

public:
	UBangCardBase();

	// 카드 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card")
	FText CardName;

	// 카드 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card")
	FText CardDescription;

	// 카드 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card")
	ECardType CardType;

	// 카드 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card")
	UTexture2D* CardIcon;
};
