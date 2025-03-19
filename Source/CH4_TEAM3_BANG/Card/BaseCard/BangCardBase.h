#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Sound/SoundBase.h"
#include "Components/StaticMeshComponent.h"
#include "BangCardBase.generated.h"

UENUM(BlueprintType)
enum class ECardType : uint8
{
	JobCard UMETA(DisplayName = "JobCard"),
	ActiveCard UMETA(DisplayName = "ActiveCard"),
	PassiveCard UMETA(DisplayName = "PassiveCard"),
	CharacterCard UMETA(DisplayName = "CharacterCard")
};

UENUM(BlueprintType)
enum class ESymbolType : uint8
{
	Heart UMETA(DisplayName = "Heart"),
	Spade UMETA(DisplayName = "Spade"),
	Clover UMETA(DisplayName = "Clover"),
	Diamond UMETA(DisplayName = "Diamond"),
	None UMETA(DisplayName = "None")
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

	// 카드 사용 시 효과음 (오디오)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card Effects")
	USoundBase* CardSoundEffect;

	// 카드에 표시될 3D 모델 (스태틱 메시)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card Effects")
	UStaticMesh* CardMesh;

	// 카드 사용 시 발생하는 파티클 효과 (파티클 시스템)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card Effects")
	UParticleSystem* CardParticleEffect;

	// 카드 심볼
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card | Symbol")
	ESymbolType SymbolType;

	// 카드 번호
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card | Symbol")
	int SymbolNumber;
};
