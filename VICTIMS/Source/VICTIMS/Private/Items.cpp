

#include "Items.h"
#include "InventoryComponent.h"

void UItems::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != Quantity)
	{	
		// 아이템 개수 업데이트 + 값 넘어가지 않게 제한 
		Quantity = FMath::Clamp(NewQuantity, 0, itemData.bStackable ? itemData.MaxStackCount : 1);

		// 만약 소유한 인벤토리라면 
		if (owningInventory)
		{
			if (Quantity <= 0)
			{
				owningInventory->RemoveSingleInstanceOfItem(this); // 아이템 개수가 0개일 때 해당 아이템 항목 제거
			}
		}
	}
}
