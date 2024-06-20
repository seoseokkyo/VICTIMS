

#include "Items.h"
#include "InventoryComponent.h"

void UItems::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != Quantity)
	{	
		// ������ ���� ������Ʈ + �� �Ѿ�� �ʰ� ���� 
		Quantity = FMath::Clamp(NewQuantity, 0, itemData.bStackable ? itemData.MaxStackCount : 1);

		// ���� ������ �κ��丮��� 
		if (owningInventory)
		{
			if (Quantity <= 0)
			{
				owningInventory->RemoveSingleInstanceOfItem(this); // ������ ������ 0���� �� �ش� ������ �׸� ����
			}
		}
	}
}
