#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Sound/SoundBase.h"
#include "Components/StaticMeshComponent.h"
#include "Data/CardEnums.h"
#include "BangCardBase.generated.h"

// Class 블루프린트에 노출시키기 위한 테그 Abstract는 선언 클레스는 제외
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, Abstract)
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
	int32 SymbolNumber;
};
