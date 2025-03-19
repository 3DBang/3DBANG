#include "BangPlayerState.h"
#include "Net/UnrealNetwork.h"

ABangPlayerState::ABangPlayerState(): PlayerStat()
{
	bReplicates = true;
}

void ABangPlayerState::BeginPlay()
{
	if (!HasAuthority())
	{
		//FPlayerStat NewStat;
		//NewStat.Value = 10;

		//Server_UpdateStruct(NewStat);
	}
}

void ABangPlayerState::Server_UpdateStruct_Implementation(const FPlayerStat& NewStruct)
{
	// 서버에서 구조체 업데이트 후 로그 출력
	PlayerStat = NewStruct;
	UE_LOG(LogTemp, Error, TEXT("Server: Updated MyStruct Value to %d"), PlayerStat.Value);
}

bool ABangPlayerState::Server_UpdateStruct_Validate(const FPlayerStat& NewStruct)
{
	// 필요 시 검증 로직 추가
	return true;
}

void ABangPlayerState::OnRep_MyStruct()
{
	//UE_LOG(LogTemp, Error, TEXT("Client: MyStruct updated to Value %d"), PlayerStat.Value);
	FString Message = FString::Printf(TEXT("Client: MyStruct updated to Value %d"), PlayerStat.Value);
	GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Yellow, Message);
}

void ABangPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// PlayerStat을 모든 클라에 복제하겠다는 뜻
	DOREPLIFETIME(ABangPlayerState, PlayerStat);
}