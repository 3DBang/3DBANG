#include "BangCardBase.h"

UBangCardBase::UBangCardBase()
{
	CardName = FText::FromString("Default Item");
	CardDescription = FText::FromString("Default Description");
	CardType = ECardType::JobCard;
	CardIcon = nullptr;
	CardSoundEffect = nullptr;
	CardMesh = nullptr;
	CardParticleEffect = nullptr;
	SymbolType = ESymbolType::Heart;
	SymbolNumber = 0;
}
