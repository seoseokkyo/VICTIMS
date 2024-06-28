

#include "InventoryComponent.h"
#include "ItemBase.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;												// Tick �ȽἭ ����
}
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


FItemAddResult UInventoryComponent::HandleAddItem(UItemBase* inputItem)
{	
	if (GetOwner())
	{
		const int32 InitialRequestedAddAmount = inputItem->Quantity;
		// ���纸�� �������� Ȯ��
		if (!inputItem->NumericData.bIsStackable)
		{
			return HandleNonStackableItems(inputItem);
		}
		// ���纸�� �����ϸ� ���纸�� �����Լ� ����
		const int32 StackableAmountAdded = HandleStackableItems(inputItem, InitialRequestedAddAmount);

		if (StackableAmountAdded == InitialRequestedAddAmount)
		{
			return FItemAddResult::AddedAll(InitialRequestedAddAmount, FText::Format(
				FText::FromString("Successfully added {0} {1} to the inventory."),
				InitialRequestedAddAmount,
				inputItem->TextData.name));
		}

		if (StackableAmountAdded < InitialRequestedAddAmount && StackableAmountAdded > 0)
		{
			return FItemAddResult::AddedPartial(StackableAmountAdded, FText::Format(
				FText::FromString("Partial amount of {0} added to the inventory. Number added = {1}"),
				inputItem->TextData.name,
				StackableAmountAdded));
		}

		if (StackableAmountAdded <= 0)
		{
			return FItemAddResult::AddedNone(FText::Format(
				FText::FromString("Couldn't add {0} to the inventory. No remaining inventory slots, or invalid item."),
				inputItem->TextData.name));
		}
	}

	check(false);
	return FItemAddResult::AddedNone(FText::FromString("TryAddItem fallthrough error. GetOwner() check somehow failed."));
}

UItemBase* UInventoryComponent::FindMatchingItem(UItemBase* itemIn) const
{	
	if (itemIn)
	{
		if (InventoryContents.Contains(itemIn))
		{
			return itemIn;
		}
	}
	return nullptr;
}

UItemBase* UInventoryComponent::FindNextItemByID(UItemBase* itemIn) const
{
	if (itemIn)
	{
		if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result = InventoryContents.FindByKey(itemIn))
		{
			return *Result;
		}
	}
	return nullptr;
}

UItemBase* UInventoryComponent::FindNextPartialStack(UItemBase* itemIn) const
{
	if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result =
		InventoryContents.FindByPredicate(
			[&itemIn](const UItemBase* InventoryItem)
			{
				return InventoryItem->ID == itemIn->ID && !InventoryItem->IsFullItemStack();
			}
		))
	{
		return *Result;
	}

	return nullptr;
}

void UInventoryComponent::RemoveSingleInstanceOfItem(UItemBase* ItemToRemove)
{
	InventoryContents.RemoveSingle(ItemToRemove);
	onInventoryUpdated.Broadcast();
}

int32 UInventoryComponent::RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove)
{
	const int32 ActualAmountToRemove = FMath::Min(DesiredAmountToRemove, ItemIn->Quantity);
	ItemIn->SetQuantity(ItemIn->Quantity - ActualAmountToRemove);
	onInventoryUpdated.Broadcast();
	return ActualAmountToRemove;
}

void UInventoryComponent::SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit)
{
	if (InventoryContents.Num() + 1 <= InventorySlotsCapacity)
	{
		RemoveAmountOfItem(ItemIn, AmountToSplit);
		AddNewItem(ItemIn, AmountToSplit);
	}
}


void UInventoryComponent::UseItem(const UItemBase* useItem)
{
	
}

void UInventoryComponent::EquipClothItem(const FItemData useItem)
{

}

void UInventoryComponent::EquipWeaponItem(const FItemData useItem)
{

}

void UInventoryComponent::UseConsumableItem(const FItemData useItem)
{

}

void UInventoryComponent::UseFurnitureItem(const FItemData useItem)
{

}

FItemAddResult UInventoryComponent::HandleNonStackableItems(UItemBase* ItemIn)
{
	// ���� �� ������ ���� ��� �߰� X
	if (InventoryContents.Num() + 1 > InventorySlotsCapacity)
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0} to the inventory. All inventory slots are full."),
			ItemIn->TextData.name));
	}

	AddNewItem(ItemIn, 1);
	return FItemAddResult::AddedAll(1, FText::Format(
	FText::FromString("Successfully added a single {0} to the inventory."), ItemIn->TextData.name));
}

int32 UInventoryComponent::HandleStackableItems(UItemBase* ItemIn, int32 RequestedAddAmount)
{
	int32 AmountToDistribute = RequestedAddAmount;
	// �߰��� �������� �̹� �κ��丮�� �ִ��� + �κ��丮�� �� ������ ������ Ȯ��
	UItemBase* ExistingItemStack = FindNextPartialStack(ItemIn);
	while (ExistingItemStack)
	{
		// ���纸�� ���ɰ������� �󸶳� �߰��� �� �ִ��� ���
		const int32 AmountToMakeFullStack = CalculateNumberForFullStack(ExistingItemStack, AmountToDistribute);

		if (AmountToDistribute <= 0)
		{
			// ������ �߰� broadcast
			onInventoryUpdated.Broadcast();
			return RequestedAddAmount;
		}
		// �� ������ �� �ִ� ������ �ִ��� Ȯ��
		ExistingItemStack = FindNextPartialStack(ItemIn);
	}
		// ���� ������ ������ ã�ƾ� �� ���
	if (InventoryContents.Num() + 1 <= InventorySlotsCapacity)
	{	
		ItemIn->SetQuantity(AmountToDistribute);
		AddNewItem(ItemIn->CreateItemCopy(), AmountToDistribute);
		return AmountToDistribute;
	}
	return 0;
}

int32 UInventoryComponent::CalculateNumberForFullStack(const UItemBase* StackableItem, int32 InitialRequestedAddAmount) const
{	
	const int32 AddAmountToMakeFullStack = StackableItem->NumericData.maxStackSize - StackableItem->Quantity;
	return FMath::Min(InitialRequestedAddAmount, AddAmountToMakeFullStack);
}

void UInventoryComponent::AddNewItem(UItemBase* Item, const int32 AmountToAdd)
{
	UItemBase* NewItem;
	if (Item->bIsCopy || Item->bIsPickup)
	{
		// �̹� �����Ǿ� �ִ���, ���� ���忡�� �ֿ���� Ȯ��
		NewItem = Item;
		NewItem->ResetItemFlags();
	}
	else
	{
		// �ٸ� �κ��丮�� �ѱ� �� 
		NewItem = Item->CreateItemCopy();
	}
	NewItem->OwningInventory = this;
	NewItem->SetQuantity(AmountToAdd);
	InventoryContents.Add(NewItem);
	onInventoryUpdated.Broadcast();
}


