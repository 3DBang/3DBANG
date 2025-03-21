#include "BangPlayerState.h"

#include "BangCharacter/BangCharacter.h"
#include "Card/BangCardManager.h"
#include "Card/ActiveCard/BangActiveCard.h"
#include "Card/BaseCard/BangCardBase.h"
#include "Card/CharacterCard/BangCharacterCard.h"
#include "Card/JobCard/BangJobCard.h"
#include "Card/PassiveCard/BangPassiveCard.h"
#include "GameMode/BangGameMode.h"
#include "Kismet/GameplayStatics.h"
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

void ABangPlayerState::AddCards_Implementation(const FPlayerCardCollection& DrawCards)
{
	//카드 매니저에 있는
	//GetCardBySymbolAndNumber 호출해서 심볼을 준다
	//카드를 넣고 플레이어에게

	PlayerInfo.MyCards.PlayerCards.Append(DrawCards.PlayerCards);
	
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	if (PlayerController)
	{
		//플레이어 컨트롤러에게 턴이 시작됐다 알림
	}
	
}

void ABangPlayerState::RemoveCards(FPlayerCardCollection& DrawCards)
{
}

void ABangPlayerState::UseCard(const FPlayerCardSymbol UseCard,const uint32 _ToUniqueID_, const EActiveType ActiveType, const EPassiveType PassiveType)
{
	// 카드가 무슨카드인지
	TObjectPtr<UBangCardManager> CardManager;
	FSingleCard SearchCard;
	
	CardManager->GetCardBySymbolAndNumberFromDataAsset(UseCard.SymbolType, UseCard.SymbolNumber, SearchCard);

	UBangCharacterCard* BangCharacterCard = Cast<UBangCharacterCard>(SearchCard.Card);

	switch (BangCharacterCard->CharacterType)
	{
	case ECharacterType::None:
		break;
	case ECharacterType::PaulRegret:
		break;
	case ECharacterType::BartCassidy:
		break;
	case ECharacterType::CalamityJanet:
		break;
	case ECharacterType::Jourdonnais:
		break;
	case ECharacterType::PedroRamirez:
		break;
	case ECharacterType::BlackJack:
		break;
	case ECharacterType::JesseJones:
		break;
	case ECharacterType::SuzyLafayette:
		break;
	case ECharacterType::SidKetchum:
		break;
	case ECharacterType::LuckyDuke:
		break;
	case ECharacterType::SlabTheKiller:
		break;
	case ECharacterType::ElGringo:
		break;
	case ECharacterType::RoseDoolan:
		break;
	case ECharacterType::WillyTheKid:
		break;
	case ECharacterType::VultureSam:
		break;
	case ECharacterType::KitCarlson:
		break;
	}
	
	switch (ActiveType)
	{
	case EActiveType::None:
		break;
	case EActiveType::Bang:
		break;
	case EActiveType::Missed:
		break;
	case EActiveType::Stagecoach:
		break;
	case EActiveType::WellsFargoBank:
		break;
	case EActiveType::Beer:
		break;
	case EActiveType::GatlingGun:
		break;
	case EActiveType::Robbery:
		break;
	case EActiveType::CatBalou:
		break;
	case EActiveType::Saloon:
		break;
	case EActiveType::Duel:
		break;
	case EActiveType::GeneralStore:
		break;
	case EActiveType::Indians:
		break;
	case EActiveType::Jail:
		break;
	case EActiveType::Dynamite:
		break;
	}

	switch (PassiveType)
	{
	case EPassiveType::None:
		break;
	case EPassiveType::Barrel:
		break;
	case EPassiveType::Scope:
		break;
	case EPassiveType::Mustang:
		break;
	case EPassiveType::Schofield:
		break;
	case EPassiveType::Volcanic:
		break;
	case EPassiveType::Remington:
		break;
	case EPassiveType::Carbine:
		break;
	case EPassiveType::Winchester:
		break;
	}
	
}

void ABangPlayerState::EndTurn(const uint32 PlayerUniqueID, ECharacterType PlayerCharacter)
{
	//내 체력보다 카드가 많으면 카드를 버리기
	if(PlayerInfo.MyCards.PlayerCards.Num() > PlayerInfo.CurrentHealth)
	{
		//버릴 카드 플레이어에게 요청
	}
	else
	{
		//아니면 턴 엔드
		ABangGameMode* BangGameMode = Cast<ABangGameMode>(GetWorld()->GetAuthGameMode());
		BangGameMode->EndTurn(PlayerUniqueID, PlayerCharacter);
	}
}

void ABangPlayerState::EndTurnReturn_Implementation(const uint32 PlayerUniqueID, ECharacterType PlayerCharacter)
{
}

void ABangPlayerState::EndTurnRemoveCards(FPlayerCardCollection& DrawCards)
{
	//카드를 제거
	ABangGameMode* BangGameMode = Cast<ABangGameMode>(GetWorld()->GetAuthGameMode());

	// 먼저 플레이어가 가지고 있는 카드와 제거할 카드를 확인
	if(PlayerInfo.MyCards.PlayerCards.Num() <= 0 || DrawCards.PlayerCards.Num() <= 0)
	{
		return; // 카드가 없으면 제거할 필요 없음
	}

	// DrawCards의 각 카드에 대해 플레이어 카드에서 제거
	for (const FPlayerCardSymbol& CardToRemove : DrawCards.PlayerCards)
	{
		PlayerInfo.MyCards.PlayerCards.RemoveSingle(CardToRemove);
	}
	
	if (!BangGameMode)
	{
		return;
	}
	
	for(FPlayerCardSymbol PlayerCard : DrawCards.PlayerCards)
	{
		BangGameMode->LooseCardFromHanded(PlayerCard.SymbolType, PlayerCard.SymbolNumber, EDeckType ::UsedCards);
	}
}

//어떤 카드 때문에 응답을 받았는지 전해주기 찬효님과 이야기 해야함
//상대 유니크 아이디
//패시브인지 액티브인지 도 보내줌
//거리 체크
void ABangPlayerState::Calculate_Distance(const FPlayerCardSymbol _UseCard_,const uint32 _ToUniqueID_, const EActiveType ActiveType, const EPassiveType PassiveType) // 상대 구분
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	
	ABangGameMode* GameMode = Cast<ABangGameMode>(World->GetAuthGameMode());
	if (GameMode == nullptr)
	{
		return;
	}
	
	//게임에 참여중인 플레이어 목록을 가지고옴
    FPlayerCollection PlayerCollection;
    GameMode->GetPlayerCollection(PlayerCollection);

	//내 인덱스
	uint32 PlayerIndex; 
	// 내 정보
	FPlayerInformation* PlayerInformation = PlayerCollection.GetPlayerInformation(PlayerInfo.PlayerUniqueID, PlayerIndex);

	// 상대방의 인덱스
	uint32 ToPlayerIndex; 
	// 상대 정보
	FPlayerInformation* ToPlayerInformation = PlayerCollection.GetPlayerInformation(_ToUniqueID_, ToPlayerIndex);

	//정보들이 없으면 리턴
	if (PlayerInformation == nullptr || ToPlayerInformation == nullptr || PlayerIndex == -1 || ToPlayerIndex == -1)
	{
		return;
	}
	
	int32 Distance = PlayerCollection.CalculateDistance(PlayerIndex,ToPlayerIndex);

	// 카드를 사용한 사람의 아이템 확인
	// 무기의 사거리에 따라 거리를 줄여줌
	// 조준경을 쓰고 있으면 거리를 줄여줌
	
	
	TObjectPtr<UBangCardManager> CardManager;
	PlayerInformation->EquippedCards.PlayerCards;
	FSingleCard searchCard;
	
	for (int32 i = 0; i < PlayerInformation->EquippedCards.PlayerCards.Num(); i++)
	{
		ESymbolType SymbolType = PlayerInformation->EquippedCards.PlayerCards[i].SymbolType;
		int32 SymbolNumber = PlayerInformation->EquippedCards.PlayerCards[i].SymbolNumber;
		
		CardManager->GetCardBySymbolAndNumberFromDataAsset(SymbolType, SymbolNumber, searchCard);

		UBangPassiveCard* BangPassiveCard = Cast<UBangPassiveCard>(searchCard.Card);
		
		switch (BangPassiveCard->PassiveType)
		{
			case EPassiveType::Scope: // Example: 조준경
				// 조준경의 효과 적용 - 거리 줄이기
				Distance -= 1;
				break;
		}
	}

	// 사용된 대상의 아이템 확인
	// 말을 타고 있으면 사거리 + 1

	// 카드를 사용한 사람의 캐릭터 확인
	// 캐릭터가 RoseDoolan이면 거리 -1
	if (PlayerInformation->CharacterCardType == ECharacterType::RoseDoolan)
	{
		Distance -= 1;
	}
	// 사용된 대상의 캐릭터 확인
	// 캐릭터가 PaulRegret이면 거리 +1
	if (ToPlayerInformation->CharacterCardType == ECharacterType::PaulRegret)
	{
		Distance += 1;
	}
	

	
	/*UseCard 를 사용하기 위해 필요한 정보
	const uint32 UniqueID, // 사용한 사람의 아이디
	const FPlayerCardSymbol& Card, // 카드 정보
	const EActiveType ActiveType, // 액티브 타입
	const EPassiveType PassiveType, // 패시브 타입
	const ECharacterType CharacterType, // 캐릭터 타입
	const uint32 ToUniqueID, // 대상
	const ECharacterType ToCharacterType // 대상 캐릭터 타입*/

	
	// 플레이어 컬렉션에서 상대 플레이어와 나의 정보를 가져와서 거리 체크
	//거리체크 후 공격을 못하는 상황이면 플레이어에게 못한다는 알림
	//_Collection 에서 상대 플레이어를 가져와서 
	//공격이 가능하면 TryApplyEffect(_UseCard, 상대플레이어 정보);

	

}


// 적용 후 컨트롤러에 호출 전달 & 컨틀
void ABangPlayerState::TryApplyEffect(TObjectPtr<UBangCardBase> _UseCard, FPlayerCollection& _Collection)
{
	
	switch (_UseCard->CardType)
	{
		case ECardType::ActiveCard: //사용카드 예) 뱅, 결투, 맥주, 역마차, 
			if(UBangActiveCard* ActiveCard= Cast<UBangActiveCard>(_UseCard))
			{
				switch (ActiveCard->ActiveType)
				{
				case EActiveType::Bang:	//뱅
					// 상대 플레이어 능력 체크 (주르도네, 뱅빗)
					// 능력이 있으면 컨트롤러에 알림
					// 상대가 카드가 있는지 확인
					// 있으면 상대방 컨트롤러에 카드를 사용할지 알림
					// 회피 카드를 사용했는지 확인
					// 체력 감소 후 상대방 플레이어에서 UpdateHp 호출 하면 체력 업데이트
					break;

				case EActiveType::Beer: //맥주
					// 사횽한 플레이어 정보에서 최대 체력을 확인
					// 최대 체력이면 사용할 수 없다고 플레이어에게 알림
					// 아니면 사횽한 플레이어 정보에서 체력 1회복
					// 체력 회복 후 UpdateHp를 써서 플레이어에게 알림
					break;

				case EActiveType::GatlingGun://개틀링건 ******** 이거 어케처리하지 빗나감 두개 어케하지
					// 전체 플레이어에게 [빗나감]카드를 내라고 알림
					// ******************뭔가 필요함 ***************************/
											
					// 없으면 체력을 1감소 시키고 UpdateHp
					// 
					break;

				case EActiveType::Robbery://강도
					// 타겟 플레이어의 카드가 없으면 사용할 수 없다고 플레이어에게 알림
					// 사용을 했을때 가져올 카드를 선택하도록 UI를 띄워줘야 하나?
					break;

				case EActiveType::CatBalou://캣블로우
					// 타겟 플레이어의 카드가 없으면 사용할 수 없다고 플레이어에게 알림
					// 사용을 했을때 제거할 카드를 선택하도록 UI를 띄워줘야 하나?
					break;

				case EActiveType::Saloon: //주점
					// 모든 플레이어를 불러서 체력회복 이 가능한지 확인후 가능하면 체력회복 후 UpdateHp
					break;

				case EActiveType::Duel: //결투
					// 상대 플레이어에게 뱅이 있는지 확인
					// 뱅이 있으면 뱅을 낼건지 플레이어에게 알림
					break;

				case EActiveType::GeneralStore: //잡화점
					// 카드 매니저에서 _Collection 크기만큼 카드를 뽑아서
					// 카드를 낸 사람부터(현재 인덱스의 사람부터) 오른쪽으로 돌림
					// 카드를 낸 사람에게 카드 선택을 알림
					break;

				case EActiveType::Indians: //인디언
					// _Collection를 통해서 모든 플레이어를 가져와서 
					// 모든 플레이어에게 카드가 있는지 확인
					// 카드가 있으면 뱅을 내라고 알림
					break;

				case EActiveType::Jail:		//감옥	//생각해보니 아래 있는것들 액티브 취급이넹  장착카드를 뭔가 바꿔야 할거 같은데 음..
					// 상대 플레이어의 장착 카드에 감옥을 추가
					// 이 감옥은 드로우 턴 전에 탈출 할수 있는지 확인
					break;

				case EActiveType::Dynamite: // 다이너마이트
					// 자신의 장착 카드에 다이너 마이트 추가
					// 트로우 턴 전에 폭발을 확인하고 다음 플레이어에게 전달
					break;

				default:
					break;
				}
			}
			break;

		case ECardType::PassiveCard: //장비카드
			if(UBangPassiveCard* PassiveCard= Cast<UBangPassiveCard>(_UseCard))
			{
				switch (PassiveCard->PassiveType)
				{
					case EPassiveType::Barrel: // 술통
						//장비칸에 술통이 있는지 확인
						//있으면 플레이어에게 있다고 알림
						//없으면 장착을 하고 장착했다는걸 플레이어에게 알림
						break;
					case EPassiveType::Scope: // 조준경
						// 장비칸에 조준경이 있는지 확인
						//있으면 플레이어에게 있다고 알림
						//없으면 장착을 하고 장착했다는걸 플레이어에게 알림
						break;
					case EPassiveType::Mustang: // 야생마
						// 장비칸에 야생마가 있는지 확인
						//있으면 플레이어에게 있다고 알림
						//없으면 장착을 하고 장착했다는걸 플레이어에게 알림
						break;
					case EPassiveType::Schofield: // 스코필드
						// 장비칸에 스코필드이 있는지 확인
						// 있으면 플레이어에게 있다고 알림
						// 없으면 장착을 하고 장착했다는걸 플레이어에게 알림
						break;
					case EPassiveType::Volcanic: // 볼캐닉
						// 장비칸에 볼캐닉이 있는지 확인
						// 있으면 플레이어에게 있다고 알림
						// 없으면 장착을 한 무기카드를 제거 후 볼캐닉을 장착 후 플레이어에게 알림
						break;
					case EPassiveType::Remington: // 레밍턴
						// 사정거리가 3으로 설정
						// 장비칸에 레밍턴이 있는지 확인
						// 있으면 플레이어에게 있다고 알림
						// 없으면 장착을 한 무기카드를 제거 후 레밍턴을 장착 후 플레이어에게 알림
						break;
					case EPassiveType::Carbine: // 카빈
						// 사정거리가 4로 설정
						// 장비칸에 레밍턴이 있는지 확인
						// 있으면 플레이어에게 있다고 알림
						// 없으면 장착을 한 무기카드를 제거 후 카빈을 장착 후 플레이어에게 알림
						break;
					case EPassiveType::Winchester: // 윈체스터
						// 사정거리가 5로 설정
						// 장비칸에 윈체스터가 있는지 확인
						// 있으면 플레이어에게 있다고 알림
						// 없으면 장착을 한 무기카드를 제거 후 윈체스터을 장착 후 플레이어에게 알림
						break;
					default:
						// 기본 처리
						break;
				}	
			}
			break;
		default: ;
	}
	
}

void ABangPlayerState::OnCharacterDrawPhase()
{
}

EJobType ABangPlayerState::GetJobType()
{
	
	return PlayerInfo.JobCardType;
}

ECharacterType ABangPlayerState::GetCharacterType()
{
	return PlayerInfo.CharacterCardType;
}

