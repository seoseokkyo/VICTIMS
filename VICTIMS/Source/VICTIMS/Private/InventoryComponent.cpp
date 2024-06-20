

#include "InventoryComponent.h"
#include "Items.h"

UInventoryComponent::UInventoryComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ����ȭ �Ǿ����� Ȯ�� 
	UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent GetIsReplicated() : %s"), GetIsReplicated() ? TEXT("TRUE") : TEXT("FALSE"));
	if (GetIsReplicated() != true)
	{

		SetIsReplicated(true);
	}

	UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent GetIsReplicated() : %s"), GetIsReplicated() ? TEXT("TRUE") : TEXT("FALSE"));
	if (GetIsReplicated() != true)
	{

		SetIsReplicated(true);
	}

}


FItemAddResult UInventoryComponent::HandleNonStackableItems(UItems* inputItem)
{
	// �ִ� �κ��丮 ���԰��� �ʰ��� ��� �߰� X, �κ��߰� ������ ������ �̸� ����� ���߿� �غ� ���� 
	if (inventoryContents.Num() + 1 > inventorySlotsCapacity)
	{
		return FItemAddResult::AddedNone(FText::FromString(TEXT("All inventory slots are full.")));
	}

	// �� ���� ����� �κ��丮�� �߰�
	AddNewItem(inputItem, 1);
	return FItemAddResult::AddedAll(1, FText::FromString(TEXT("Successfully added a single {0} to the inventory.")));
}

int32 UInventoryComponent::HandleStackableItems(UItems* inputItem, int32 requestedAddAmount)
{
	return 0;
}

int32 UInventoryComponent::CalculateNumberForFullStack(UItems* StackableItem, int32 InitialRequestedAddAmount)
{
	const int32 addMountToMakeFullStack = StackableItem->itemData.MaxStackCount - StackableItem->Quantity; // ������ �����ͼ� (�ִ���� - �������)
	return FMath::Min(InitialRequestedAddAmount, addMountToMakeFullStack); // �� �� �� ���� �� ��ȯ
}

void UInventoryComponent::AddNewItem(UItems* item, const int32 amountToAdd)
{
	UItems* newItem;
	newItem = item;

	newItem->owningInventory = this; // ������ �κ��丮�� ������ �߰� 
	newItem->SetQuantity(amountToAdd); // �߰��� ���� ���� 
	inventoryContents.Add(newItem); // �κ��丮 ������ �迭���� �������׸� �߰�
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

FItemAddResult UInventoryComponent::HandleAddItem(UItems* inputItem)
{
	if (GetOwner())
	{
		// ������ ���� üũ
		const int32 initialRequestedAddAmount = inputItem->Quantity;
		// ���纸�� �Ұ����� �� ĭ ���� �������� ���, ���� ó�� 
		if (!inputItem->itemData.bStackable)
		{
			return HandleNonStackableItems(inputItem);
		}

		// ���� ������ ���, �� �߰�
		const int32 stackableAmountAdded = HandleStackableItems(inputItem, initialRequestedAddAmount);

		if (stackableAmountAdded == initialRequestedAddAmount)
		{
			return FItemAddResult::AddedAll(initialRequestedAddAmount, FText::Format(FText::FromString("Successfully added {0} {1} to the inventory."), initialRequestedAddAmount, inputItem->itemData.name));
		}

		if (stackableAmountAdded < initialRequestedAddAmount && stackableAmountAdded > 0)
		{
			return FItemAddResult::AddedPartial(stackableAmountAdded, FText::Format(FText::FromString("Partial amount of {0} added to the inventory. Number added = {1}"), inputItem->itemData.name, stackableAmountAdded));
		}

		if (stackableAmountAdded <= 0) // �߰��� ���� 0���� ������ �ȵ�����
		{
			return FItemAddResult::AddedNone(FText::Format(FText::FromString("Couldn't add {0} to the inventory. No remaining inventory slots, or invalid item."), inputItem->itemData.name));
		}

	}
	return FItemAddResult();
}

UItems* UInventoryComponent::FindMatchingItem(UItems* itemIn) const
{
	if (itemIn) // �ִ��� Ȯ�� ����
	{
		if (inventoryContents.Contains(itemIn))
		{
			return itemIn;
		}
	}
	return nullptr;
}

void UInventoryComponent::RemoveSingleInstanceOfItem(UItems* itemToRemove)
{
	inventoryContents.RemoveSingle(itemToRemove); // ������ �׸� 1�� ����
}

int32 UInventoryComponent::RemoveAmountOfItem(UItems* itemToRemove, int32 desiredAmountToRemove)
{
	const int32 actualAmountToRemove = FMath::Min(desiredAmountToRemove, itemToRemove->Quantity); // �ش� �׸��� �� ����
	itemToRemove->SetQuantity(itemToRemove->Quantity - actualAmountToRemove); // ���� ������ ���� ����, ���� ���� + �׸� �������� ǥ��
	return actualAmountToRemove; // ������ ���ŵ� ���� ��ȯ
}

