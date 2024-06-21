

#include "ItemBase.h"


UItemBase::UItemBase()
{

}

void UItemBase::SetQuantity(const int32 newQuantity)
{

	if (newQuantity != quantity)
	{   // 적재보관 가능/불가능 조건으로 적재개수 제한걸기
		quantity = FMath::Clamp(newQuantity, 0, numericData.bIsStackable ? numericData.maxStackCount : 1);
		/*
		if(owningInventory)
		{
			if(quantity <= 0)
			{
				owningInventory->RemoveItem(this);
			}
		}
		*/
	}
}

void UItemBase::Use(class AVICTIMSCharacter* character)
{

}
