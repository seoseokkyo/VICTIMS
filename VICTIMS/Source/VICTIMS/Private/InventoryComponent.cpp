

#include "InventoryComponent.h"
#include "Items.h"

UInventoryComponent::UInventoryComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// 동기화 되었는지 확인 
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
	// 최대 인벤토리 슬롯개수 초과일 경우 추가 X, 인벤추가 실패한 아이템 이름 출력은 나중에 해볼 예정 
	if (inventoryContents.Num() + 1 > inventorySlotsCapacity)
	{
		return FItemAddResult::AddedNone(FText::FromString(TEXT("All inventory slots are full.")));
	}

	// 위 조건 통과시 인벤토리에 추가
	AddNewItem(inputItem, 1);
	return FItemAddResult::AddedAll(1, FText::FromString(TEXT("Successfully added a single {0} to the inventory.")));
}

int32 UInventoryComponent::HandleStackableItems(UItems* inputItem, int32 requestedAddAmount)
{
	return 0;
}

int32 UInventoryComponent::CalculateNumberForFullStack(UItems* StackableItem, int32 InitialRequestedAddAmount)
{
	const int32 addMountToMakeFullStack = StackableItem->itemData.MaxStackCount - StackableItem->Quantity; // 데이터 가져와서 (최대수량 - 현재수량)
	return FMath::Min(InitialRequestedAddAmount, addMountToMakeFullStack); // 둘 중 더 작은 값 반환
}

void UInventoryComponent::AddNewItem(UItems* item, const int32 amountToAdd)
{
	UItems* newItem;
	newItem = item;

	newItem->owningInventory = this; // 소유한 인벤토리에 아이템 추가 
	newItem->SetQuantity(amountToAdd); // 추가할 개수 설정 
	inventoryContents.Add(newItem); // 인벤토리 아이템 배열에도 아이템항목 추가
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

FItemAddResult UInventoryComponent::HandleAddItem(UItems* inputItem)
{
	if (GetOwner())
	{
		// 아이템 수량 체크
		const int32 initialRequestedAddAmount = inputItem->Quantity;
		// 적재보관 불가능한 한 칸 차지 아이템일 경우, 따로 처리 
		if (!inputItem->itemData.bStackable)
		{
			return HandleNonStackableItems(inputItem);
		}

		// 적재 가능할 경우, 값 추가
		const int32 stackableAmountAdded = HandleStackableItems(inputItem, initialRequestedAddAmount);

		if (stackableAmountAdded == initialRequestedAddAmount)
		{
			return FItemAddResult::AddedAll(initialRequestedAddAmount, FText::Format(FText::FromString("Successfully added {0} {1} to the inventory."), initialRequestedAddAmount, inputItem->itemData.name));
		}

		if (stackableAmountAdded < initialRequestedAddAmount && stackableAmountAdded > 0)
		{
			return FItemAddResult::AddedPartial(stackableAmountAdded, FText::Format(FText::FromString("Partial amount of {0} added to the inventory. Number added = {1}"), inputItem->itemData.name, stackableAmountAdded));
		}

		if (stackableAmountAdded <= 0) // 추가된 값이 0보다 작으면 안되지만
		{
			return FItemAddResult::AddedNone(FText::Format(FText::FromString("Couldn't add {0} to the inventory. No remaining inventory slots, or invalid item."), inputItem->itemData.name));
		}

	}
	return FItemAddResult();
}

UItems* UInventoryComponent::FindMatchingItem(UItems* itemIn) const
{
	if (itemIn) // 있는지 확인 먼저
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
	inventoryContents.RemoveSingle(itemToRemove); // 아이템 항목 1개 제거
}

int32 UInventoryComponent::RemoveAmountOfItem(UItems* itemToRemove, int32 desiredAmountToRemove)
{
	const int32 actualAmountToRemove = FMath::Min(desiredAmountToRemove, itemToRemove->Quantity); // 해당 항목의 양 제거
	itemToRemove->SetQuantity(itemToRemove->Quantity - actualAmountToRemove); // 실제 제거할 수량 빼고, 수량 설정 + 항목 수량으로 표시
	return actualAmountToRemove; // 실제로 제거된 수량 반환
}

