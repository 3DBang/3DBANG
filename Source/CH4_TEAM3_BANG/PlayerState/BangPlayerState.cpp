#include "BangPlayerState.h"

#include "Card/BangCardManager.h"
#include "Card/ActiveCard/BangActiveCard.h"
#include "Card/BaseCard/BangCardBase.h"
#include "Net/UnrealNetwork.h"

ABangPlayerState::ABangPlayerState(): PlayerStat()
{
	bReplicates = true;
}

void ABangPlayerState::BeginPlay()
{
	Super::BeginPlay();

	
	if (!HasAuthority())
	{
		InitPlayerInfo();
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

void ABangPlayerState::AddDrawnCards(FCardCollection& DrawCards)
{
	if (HasAuthority())
	{
		PlayerInfo.MyCards.Append(DrawCards.CardList);

		//카드를 넣고 플레이어에게
		
		APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
		if (PlayerController)
		{
			//플레이어 컨트롤러 로직
		}
	}
}

void ABangPlayerState::TryApplyEffect(TObjectPtr<UBangCardBase> _UseCard)
{
	
	switch (_UseCard->CardType)
	{
	case ECardType::ActiveCard: //사용카드 예) 뱅, 결투, 맥주, 역마차, 
		if(UBangActiveCard* Card= Cast<UBangActiveCard>(_UseCard))
		{
			// EActiveType::Bang -> 뱅 (공격 카드)
			// EActiveType::Missed -> 빗나감 (방어 카드)
			// EActiveType::Stagecoach -> 역마차 (카드 2장 뽑기)
			// EActiveType::WellsFargoBank -> 웰스파고은행 (카드 3장 뽑기)
			// EActiveType::Beer -> 맥주 (체력 회복)
			// EActiveType::GatlingGun -> 기관총 (모든 플레이어 공격)
			// EActiveType::Robbery -> 강탈 (카드 1장 훔치기)
			// EActiveType::CatBalou -> 캣벌로우 (카드 1장 버리기)
			// EActiveType::Saloon -> 주점 (모든 플레이어 체력 회복)
			// EActiveType::Duel -> 결투 (뱅을 연속으로 내야 함)
			// EActiveType::GeneralStore -> 잡화점 (모든 플레이어가 카드 선택)
			// EActiveType::Indians -> 인디언 (모든 플레이어가 뱅 내야 함)
			// EActiveType::Jail -> 감옥 (턴 제한)
			// EActiveType::Dynamite -> 다이너마이트 (폭발 시 피해)
			switch (Card->ActiveType)
			{
			case EActiveType::Bang:
				// 플레이어 능력 체크 (주르도네, 뱅빗)
					//능력이 있으면 컨트롤러에 알림
				// 내 카드더미에서 빗나감이 있는지 확인
					//있으면 컨트롤러에 알림
				// 체력 감소
					//컨트롤러에 알림?
				break;

			case EActiveType::Stagecoach:
				//카드 두개 뽑기
					//카드 두개 뽑기?
				break;

			case EActiveType::WellsFargoBank:
				//카드 두개 뽑기
					//카드 세개 뽑기?
				break;

			case EActiveType::Beer:
				// 체력 1 회복
				break;

			case EActiveType::GatlingGun:
				// Implement logic for Gatling Gun card effect
				break;

			case EActiveType::Robbery:
				// Implement logic for Robbery card effect
				break;

			case EActiveType::CatBalou:
				// Implement logic for Cat Balou card effect
				break;

			case EActiveType::Saloon:
				// Implement logic for Saloon card effect
				break;

			case EActiveType::Duel:
				// Implement logic for Duel card effect
				break;

			case EActiveType::GeneralStore:
				// Implement logic for General Store card effect
				break;

			case EActiveType::Indians:
				// Implement logic for Indians card effect
				break;

			case EActiveType::Jail:
				// Implement logic for Jail card effect
				break;

			case EActiveType::Dynamite:
				// Implement logic for Dynamite card effect
				break;

			default:
				break;
			}
		}
		break;

	case ECardType::PassiveCard: //장비카드
		//현재 장착된 장비인지 확인
		break;
	}
	
}

void ABangPlayerState::OnCharacterDrawPhase()
{
}

