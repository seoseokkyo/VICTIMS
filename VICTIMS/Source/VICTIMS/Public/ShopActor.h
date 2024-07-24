
#pragma once

#include "CoreMinimal.h"
#include "Item/UsableActor.h"
#include "InventoryInterface.h"
#include "InventoryComponent.h"
#include "FShopList.h"
#include "ShopActor.generated.h"

UCLASS()
class VICTIMS_API AShopActor : public AUsableActor, public IInventoryInterface
{
	GENERATED_BODY()

public:	
	AShopActor();

	virtual bool OnActorUsed_Implementation(APlayerController* Controller) override;

	// INTERFACE // 
	void GetShopProperties_Implementation(FName& Namee, uint8& NumberOfRows, uint8& SlotsPerRow, bool& IsShop, uint8& InventorySize);
	virtual void GetShopInventory_Implementation(UInventoryComponent*& OutInventoryComponent) override;
	virtual bool GetCanShopItems_Implementation() override;
	virtual bool ShopLooted_Implementation() override;
	virtual TArray<APlayerState*> GetPlayersViewing_Implementation() override;
	//===========//

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
	bool LoadInventoryItems(uint8 Size, TArray<FSlotStructure> Items);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName = "Name", Category = "Properties"))
	FName S_Name;
	uint8 S_NumberOfRows;
	uint8 S_SlotsPerRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName = "Can Shop Items", Category = "Properties"))
	bool S_CanShopItems;

	uint8 S_InventorySize;

//========================================================================================================================================

	UFUNCTION()
	TArray<FShopList> GetShopList();
	UFUNCTION()
	TArray<FSlotStructure> GetShopItems();
	UFUNCTION()
	void SetItemAmount(FSlotStructure& Item, uint8 NewAmount);
	UFUNCTION()
	uint8 GetItemMaxStackSize(const FSlotStructure Item);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 ShopItemAmount = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* DB_ItemList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* BP_ShopDB;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* Body;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* Chest;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* Feet;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* Hands;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* Legs;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* Head;

// 상점 종류 나누기 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop Settings")
	bool bIsConsumableShop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop Settings")
	bool bIsEquipmentShop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop Settings")
	bool bIsFurnitureShop;





	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	TArray<FSlotStructure> InventoryItems;

 };
