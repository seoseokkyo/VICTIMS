

#include "PickUp.h"
#include "InventoryComponent.h"
#include "ItemBase.h"

APickUp::APickUp()
{
	PrimaryActorTick.bCanEverTick = false;									// Tick 안써서 꺼둠
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetSimulatePhysics(true);
	SetRootComponent(PickupMesh);
}
void APickUp::BeginPlay()
{
	Super::BeginPlay();
	InitializePickup(ItemQuantity);
}

void APickUp::InitializePickup(const int32 InQuantity)
{
	if (!ItemRowHandle.IsNull())
	{
		const FItemData* ItemData = ItemRowHandle.GetRow<FItemData>(ItemRowHandle.RowName.ToString());
		if(ItemData)
		{
			ItemReference = NewObject<UItemBase>(this, UItemBase::StaticClass());

			ItemReference->ID = ItemData->ID;
			ItemReference->ItemType = ItemData->itemType;
			ItemReference->ItemQuality = ItemData->itemQuality;
			ItemReference->NumericData = ItemData->numericData;
			ItemReference->TextData = ItemData->textData;
			ItemReference->AssetData = ItemData->assetData;

			ItemReference->NumericData.bIsStackable = ItemData->numericData.maxStackSize > 1;
			InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);

			PickupMesh->SetStaticMesh(ItemData->assetData.staticMesh);
			UpdateInteractableData();
		}
	}
}

void APickUp::InitializeDrop(UItemBase* ItemToDrop, const int32 InQuantity)
{
	ItemReference = ItemToDrop;
	InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);
	ItemReference->OwningInventory = nullptr;
	PickupMesh->SetStaticMesh(ItemToDrop->AssetData.staticMesh);

	UpdateInteractableData();
}

void APickUp::BeginFocus()
{	
	// 상호작용 가능 범위안에 있으면 아웃라인 그리기
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(true);
	}
}

void APickUp::EndFocus()
{
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(false);
	}
}
void APickUp::Interact(AVICTIMSCharacter* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		TakePickup(PlayerCharacter);
	}
}

void APickUp::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractableType::Pickup;
	InstanceInteractableData.Action = ItemReference->TextData.interactionText;
	InstanceInteractableData.Name = ItemReference->TextData.name;
	InstanceInteractableData.Quantity = ItemReference->Quantity;
	InteractableData = InstanceInteractableData;
}

void APickUp::TakePickup(const AVICTIMSCharacter* Taker)
{
	if (!IsPendingKillPending())
	{
		if (ItemReference)
		{
			if (UInventoryComponent* PlayerInventory = Taker->GetInventory())
			{
				const FItemAddResult AddResult = PlayerInventory->HandleAddItem(ItemReference);

				switch (AddResult.operationResult)
				{
				case EItemAddResult::IAR_NoItemAdded:
					break;
				case EItemAddResult::IAR_PartialAmountItemAdded:
					UpdateInteractableData();
					Taker->UpdateInteractionWidget();
					break;
				case EItemAddResult::IAR_AllItemAdded:
					Destroy();
					break;
				default:
					break;
				}

				UE_LOG(LogTemp, Warning, TEXT("%s"), *AddResult.resultMessage.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Player inventory component is null!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Pickup internal item reference was somehow null!"));
		}
	}
}

#if WITH_EDITOR
void APickUp::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(FDataTableRowHandle, RowName))
	{
		if (!ItemRowHandle.IsNull())
		{
			const FItemData* ItemData = ItemRowHandle.GetRow<FItemData>(ItemRowHandle.RowName.ToString());
			PickupMesh->SetStaticMesh(ItemData->assetData.staticMesh);
		}
	}
}
#else

#endif
