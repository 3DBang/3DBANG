#pragma once

#include "CoreMinimal.h"
#include "CardEnums.generated.h"

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

UENUM(BlueprintType)
enum class EJobType : uint8
{
	Officer UMETA(DisplayName = "Officer"),
	SubOfficer UMETA(DisplayName = "SubOfficer"),
	Outlaw UMETA(DisplayName = "Outlaw"),
	Betrayer UMETA(DisplayName = "Betrayer")
};

UENUM(BlueprintType)
enum class EActiveType : uint8
{
	Bang UMETA(DisplayName = "Bang"),						// 뱅 (공격 카드)
	Missed UMETA(DisplayName = "Missed"),					// 빗나감 (방어 카드)
	Stagecoach UMETA(DisplayName = "Stagecoach"),			// 역마차 (카드 2장 뽑기)
	WellsFargoBank UMETA(DisplayName = "Wells Fargo Bank"), // 웰스파고은행 (카드 3장 뽑기)
	Beer UMETA(DisplayName = "Beer"),						// 맥주 (체력 회복)
	GatlingGun UMETA(DisplayName = "Gatling Gun"),			// 기관총 (모든 플레이어 공격)
	Robbery UMETA(DisplayName = "Robbery"),					// 강탈 (카드 1장 훔치기)
	CatBalou UMETA(DisplayName = "Cat Balou"),				// 캣벌로우 (카드 1장 버리기)
	Saloon UMETA(DisplayName = "Saloon"),					// 주점 (모든 플레이어 체력 회복)
	Duel UMETA(DisplayName = "Duel"),						// 결투 (뱅을 연속으로 내야 함)
	GeneralStore UMETA(DisplayName = "General Store"),		// 잡화점 (모든 플레이어가 카드 선택)
	Indians UMETA(DisplayName = "Indians"),					// 인디언 (모든 플레이어가 뱅 내야 함)
	Jail UMETA(DisplayName = "Jail"),						// 감옥 (턴 제한)
	Dynamite UMETA(DisplayName = "Dynamite")				// 다이너마이트 (폭발 시 피해)
};

UENUM(BlueprintType)
enum class EPassiveType : uint8
{
	Barrel UMETA(DisplayName = "Barrel"),          // 술통
	Scope UMETA(DisplayName = "Scope"),            // 조준경
	Mustang UMETA(DisplayName = "Mustang"),        // 야생마
	Schofield UMETA(DisplayName = "Schofield"),    // 스코필드
	Volcanic UMETA(DisplayName = "Volcanic"),      // 볼캐닉
	Remington UMETA(DisplayName = "Remington"),    // 레밍턴
	Carbine UMETA(DisplayName = "Carbine"),        // 카빈
	Winchester UMETA(DisplayName = "Winchester")   // 윈체스터
};

UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	PaulRegret UMETA(DisplayName = "Paul Regret"),         // 폴 리그레트
	BartCassidy UMETA(DisplayName = "Bart Cassidy"),       // 바트 캐시디
	CalamityJanet UMETA(DisplayName = "Calamity Janet"),   // 캘러미티 자넷
	Jourdonnais UMETA(DisplayName = "Jourdonnais"),        // 주르도네
	
	PedroRamirez UMETA(DisplayName = "Pedro Ramirez"),     // 페드로 라미레즈
	BlackJack UMETA(DisplayName = "Black Jack"),          // 블랙 잭
	JesseJones UMETA(DisplayName = "Jesse Jones"),        // 제시 존스
	SuzyLafayette UMETA(DisplayName = "Suzy Lafayette"),  // 수지 라파에트
	SidKetchum UMETA(DisplayName = "Sid Ketchum"),        // 시드 케첨
	LuckyDuke UMETA(DisplayName = "Lucky Duke"),          // 럭키 듀크
	
	SlabTheKiller UMETA(DisplayName = "Slab the Killer"), // 슬랩 더 킬러
	ElGringo UMETA(DisplayName = "El Gringo"),           // 엘 그링고
	RoseDoolan UMETA(DisplayName = "Rose Doolan"),       // 로즈 둘란
	WillyTheKid UMETA(DisplayName = "Willy the Kid"),    // 윌리 더 키드

	VultureSam UMETA(DisplayName = "Vulture Sam")        // 벌쳐 샘
};