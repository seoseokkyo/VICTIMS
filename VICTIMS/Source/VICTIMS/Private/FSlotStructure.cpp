

#include "FSlotStructure.h"

FSlotStructure::FSlotStructure()
{
	itemStructure = {};
	amount = 0;
}

void FSlotStructure::InitSlot(const FItemData& inItemStructure, const uint8& inAmount)
{
	itemStructure = inItemStructure;
	amount = inAmount;
}
