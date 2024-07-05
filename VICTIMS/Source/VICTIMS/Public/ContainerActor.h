
#pragma once

#include "CoreMinimal.h"
#include "InventoryInterface.h"
#include "InventoryComponent.h"
#include "UsableActor.h"
#include "ContainerActor.generated.h"


UCLASS()
class VICTIMS_API AContainerActor : public AUsableActor, public IInventoryInterface
{
	GENERATED_BODY()

public:
	AContainerActor();
	
 	virtual bool OnActorUsed_Implementation(APlayerController* Controller) override;
	
 	/* Interface */
	virtual void GetContainerProperties_Implementation(FName& Namee, uint8& NumberOfRows, uint8& SlotsPerRow, bool& IsStorageContainer, uint8& InventorySize) override;
	virtual void GetContainerInventory_Implementation(UInventoryComponent*& OutInventoryComponent) override;
	virtual TArray<APlayerState*> GetPlayersViewing_Implementation() override;
	virtual bool ContainerLooted_Implementation() override;
	virtual bool GetCanStoreItems_Implementation() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInventoryComponent* InventoryComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<APlayerState*> PlayersViewing;
	
protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	virtual bool InitializeInventory();
	
	UFUNCTION()
	bool LoadInventoryItems(uint8 Size, TArray<FSlotStructure> InventoryItems);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Name", Category="Properties"))
	FName C_Name;

	uint8 C_NumberOfRows;
	uint8 C_SlotsPerRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Can Store Items", Category="Properties"))
	bool C_CanStoreItems;

	uint8 C_InventorySize;
};
