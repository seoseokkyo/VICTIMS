

#include "ItemBase.h"


UItemBase::UItemBase()
{

}

void UItemBase::SetQuantity(const int32 newQuantity)
{

	if (newQuantity != quantity)
	{   // ���纸�� ����/�Ұ��� �������� ���簳�� ���Ѱɱ�
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
