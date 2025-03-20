#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BangPlayerState.generated.h"

struct FCardCollection;
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
	TObjectPtr<UBangJobCard> JobCard;
	
	//캐릭터 카드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	TObjectPtr<UBangCharacterCard> CharacterCard;
	
	//보유한 카드(사용가능한 카드)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	TArray<TObjectPtr<UBangCardBase>> MyCards;
	
	//장착된 카드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	TArray<TObjectPtr<UBangCardBase>> EquippedCards;
};

UCLASS()
class CH4_TEAM3_BANG_API ABangPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ABangPlayerState();

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Player State")
	void InitPlayerInfo();
	
	UPROPERTY(Replicated)
	FPlayerInfo PlayerInfo;

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

	//카드를 넣어주는 함수
	void AddDrawnCards(FCardCollection& DrawCards);

	// 카드의 사용 조건이 맞을때 효과를 적용하는 함수
	// 효과를 발생시킨 플레이어 , 사용한 카드
	void TryApplyEffect(TObjectPtr<UBangCardBase> _UseCard);

	//플레이어가 드로우 할턴에 뭔가를 해야하는 자기 캐릭터가 있으면 그 캐릭터 일때 플레이어 컨트롤러로 보내줌
	// 걔 UI 띄워라 
    void OnCharacterDrawPhase();

};


