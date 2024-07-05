#include "FSlotStructure.h"
#include "EAttributes.h"

FSlotStructure::FSlotStructure()
{
	ItemStructure = {};
	Amount = 0;
}

void FSlotStructure::InitSlot(const FItemStructure& InItemStructure, const uint8& InAmount)
{
	ItemStructure = InItemStructure;
	Amount = InAmount;
}
uint8 FSlotStructure::GetAttributeValueByAttribute(EAttributes Attribute)
{
	switch (Attribute)
	{
	case EAttributes::Strength:
		return ItemStructure.Strength;
	default:
		return 0;
	}
}
