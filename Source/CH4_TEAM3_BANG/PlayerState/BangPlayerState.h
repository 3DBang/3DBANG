#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BangPlayerState.generated.h"

class UBangJobCard;
class UBangCharacterCard;
class UBangCardBase;

USTRUCT()
struct FPlayerStat
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 Value;

	UPROPERTY()
	float Health;

	UPROPERTY()
	int32 Score;

	// NetSerialize 함수: 읽기/쓰기를 동일 순서로 처리해야 합니다.
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << Value;
		Ar << Health;
		Ar << Score;
		bOutSuccess = true;
		return true;
	}
};

template<>
struct TStructOpsTypeTraits<FPlayerStat> : public TStructOpsTypeTraitsBase2<FPlayerStat>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	// 플레이어가 가지는 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	int32 MaxHealth;

	// 플레이어 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	int32 CurrentHealth;

	// 나를 볼 때 사거리 (다른 플레이어 기준)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	int32 RangeToMe;

	// 내가 볼 때 사거리 (내 기준)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	int32 RangeFromMe;

	//역할 카드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	UBangJobCard* JobCard;

	//캐릭터 카드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	UBangCharacterCard* CharacterCard;
	
	//보유한 카드(사용가능한 카드)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	TArray<UBangCardBase*> MyCards;
	
	//장착된 카드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	TArray<UBangCardBase*> EquippedCards;
};

UCLASS()
class CH4_TEAM3_BANG_API ABangPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ABangPlayerState();

	virtual void BeginPlay() override;

	// FPlayerStat를 복제할 때 OnRep_MyStruct 함수 호출 (동기화)
	UPROPERTY(ReplicatedUsing = OnRep_MyStruct)
	FPlayerStat PlayerStat;

	// ex) 클라이언트가 서버에 새로운 데이터를 전달하는 RPC 함수 
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateStruct(const FPlayerStat& NewStruct);
	
	// 복제 변수 업데이트 시 클라이언트에서 호출
	/*
	NetSerialize는 복제 시스템 내부에서 호출되어 데이터를 직렬화하는 역할을 합니다.
	따라서 직접 호출하는 코드가 없더라도, 클라이언트에서 실제로 값이 제대로 업데이트되고 OnRep 함수가 호출된다면,
	NetSerialize가 정상적으로 작동하고 있다는 간접 증거입니다.
	 */
	UFUNCTION()
	void OnRep_MyStruct();

	// 복제 변수 등록
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	//CardManager->HandCards()를 했을때
	//카드를 받았을때 컨트롤러로 연결
	
	//공격을 하겠다 했을때 공격이 가능한지 확인
	
	
};


