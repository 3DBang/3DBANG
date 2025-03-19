#include "BangCardBase.h"

UBangCardBase::UBangCardBase()
{
	CardName = FText::FromString("Default Item");
	CardDescription = FText::FromString("Default Description");
	CardType = ECardType::JobCard;
	CardIcon = nullptr;
}
