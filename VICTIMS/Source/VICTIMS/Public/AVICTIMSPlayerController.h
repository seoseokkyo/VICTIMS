
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HUDInterface.h"
#include "AVICTIMSPlayerController.generated.h"

class UInventoryManagerComponent;
class UEquipmentComponent;
class UDatatable;
class UItembase;
class AVICTIMSCharacter;
class UInventoryComponent;

UCLASS()
class VICTIMS_API AVICTIMSPlayerController : public APlayerController, public IHUDInterface
{
	GENERATED_BODY()

protected:
	uint8 MaximumHotbarSlots = 10;

	virtual void BeginPlay() override;

public: 
	AVICTIMSPlayerController();

	UFUNCTION(BlueprintCallable, Category = "Runtime Inspector")		
	int GetCurrentViewMode(const APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category="Character")
	void EnableUIMode();
	UFUNCTION(BlueprintCallable, Category="Character")
	void DisableUIMode();

	UFUNCTION()
	UDataTable* GetItemDB();

	uint8 GetMaximumHotbarSlots() { return MaximumHotbarSlots; };

	uint8 MenuAnchorIndex;

	FReturnTupleSlotNativeID MenuAnchorSlotIndex;



	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(Category="Inventory", OverrideNativeName="InventoryComponent"))
	UInventoryManagerComponent* inventoryManagerComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(Category="Inventory", OverrideNativeName="InventoryComponent"))
	UEquipmentComponent* playerInventoryComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AMainHUD* HUD_Reference;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMainMenu* HUDLayoutReference;


	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)	
	AVICTIMSCharacter* victimsPlayer;

	uint8 UIGetPlayerGold();

	UFUNCTION()
	void SetupHUDReferences();

	UFUNCTION()
	bool IsContainerOpen();

	void SetInputDependingFromVisibleWidgets();
	UFUNCTION()
	void SetMouseToCenterPosition();
	
	// ========================================================================================================
	// 인터페이스 
	virtual void UI_MoveInventoryItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot) override;
	virtual void UI_DropInventoryItem_Implementation(const uint8& InventorySlot) override;
	virtual void UI_EquipInventoryItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot) override;
	virtual void UI_UnEquipInventoryItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot) override;
	virtual void UI_UseInventoryItem_Implementation(const uint8& InventorySlot) override;
	virtual void UI_UseContainerItem_Implementation(const uint8& InventorySlot) override;
	virtual void UI_UseHotbarItem_Implementation(const uint8& InventorySlot) override;
	virtual void UI_TakeContainerItem_Implementation(const uint8& FromContainerSlot, const uint8& ToInventorySlot) override;
	virtual void UI_DepositContainerItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot) override;
	virtual void UI_MoveContainerItem_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot) override;
	virtual void UI_EquipFromContainer_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot) override;
	virtual void UI_UnEquipToContainer_Implementation(const uint8& FromInventorySlot, const uint8& ToInventorySlot) override;
	virtual void UI_MoveHotbarItem_Implementation(const uint8& FromSlot, const uint8& ToSlot, const bool IsDraggedFromInventory, const bool IsDraggedFromHotbar) override;
	// ========================================================================================================

};
