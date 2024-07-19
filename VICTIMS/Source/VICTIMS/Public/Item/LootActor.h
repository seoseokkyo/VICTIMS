
#pragma once

#include "CoreMinimal.h"
#include "ContainerActor.h"
#include "FLootList.h"
#include "LootActor.generated.h"


UCLASS()
class VICTIMS_API ALootActor : public AContainerActor
{
	GENERATED_BODY()

public:
	ALootActor();

	virtual bool InitializeInventory() override;

	UFUNCTION()
	TArray<FLootList> GetLootList();
	UFUNCTION()
	TArray<FSlotStructure> GetRandomLootItems();
	UFUNCTION()
	void SetItemAmount(FSlotStructure& Item, uint8 NewAmount);

	UFUNCTION()
	bool IsItemCurrency(const FSlotStructure Item);
	UFUNCTION()
	uint8 GetItemMaxStackSize(const FSlotStructure Item);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 MinLootItems = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 MaxLootItems = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* DB_ItemList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* BP_LootDB;

protected:
	virtual void BeginPlay() override;

private:
	TArray<FSlotStructure> InventoryItems;


};
