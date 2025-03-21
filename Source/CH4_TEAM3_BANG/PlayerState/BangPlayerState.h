#pragma once

#include "CoreMinimal.h"
#include "Data/PlayerInformation.h"
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


UCLASS()
class CH4_TEAM3_BANG_API ABangPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ABangPlayerState();

	virtual void BeginPlay() override;

	//블루프린트에서 플레이어 정보를 설정이 필요할때 부르는 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Player State")
	void InitPlayerInfo();
	
	UPROPERTY(Replicated)
	FPlayerInformation PlayerInfo;

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

	//카드를 버리는 함수


	// 플레이어 컨트롤러에서 호출하는 함수
	// 카드를 사용했을때 적과 나의 거리를 체크하는 함수
	// @ 상대방의 정보 ex, 스테이트 or 플레이어 인포
	// @ FPlayerCardCollection _UseCard 사용한 카드
	// @ FPlayerInformation 플레이어 위치정보
	void Calculate_Distance(TObjectPtr<UBangCardBase> _UseCard, FPlayerCollection& _Collection);
	
	// 카드의 사용 조건이 맞을때 효과를 적용하는 함수
	// 효과를 발생시킨 플레이어 , 사용한 카드
	void TryApplyEffect(TObjectPtr<UBangCardBase> _UseCard, FPlayerCollection& _Collection);

	//플레이어가 드로우 할턴에 뭔가를 해야하는 자기 캐릭터가 있으면 그 캐릭터 일때 플레이어 컨트롤러로 보내줌
	// 걔 UI 띄워라 
    void OnCharacterDrawPhase();


	//getJobType
	EJobType GetJobType();
	
	//getCharacterType
	ECharacterType GetCharacterType();
	//GetCharacterType

	////// 필요 유틸

	// @ int16 현재 플레이어 인덱스
	// @ int16 상대 플레이어 인덱스
	// @ FPlayerCollection& _Collection
	//_Collection에서 특정한 플레이어 인덱스를 가지고 와서 반환하는 함수

	//_Collection 전체를 돌면서 모든 플레이어에게 특정 함수를 실행하는 함수 템플릿 함수로 만들어야 할듯?
};


