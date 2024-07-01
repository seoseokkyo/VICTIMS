
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class UButton;
class UBorder;
class UProfilePanel;
class UInventoryPanel;
class UContainerPanel;
class UHotbarPanel;
class AVICTIMSCharacter;

UENUM()
enum class ELayout : int8
{
	Undefiend = -1		UMETA(DisplayName = "Undefined"),
	Inventory			UMETA(DisplayName = "Inventory"),
	Equipment			UMETA(DisplayName = "Equipment"),
	Container			UMETA(DisplayName = "Container"),
	Store				UMETA(DisplayName = "Store")
};

UCLASS()
class VICTIMS_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY()
	AVICTIMSCharacter* PlayerCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* Btn_Quit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UBorder* Border_Profile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UBorder* Border_Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UBorder* Border_Container;

// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))			// ªÛ¿Œ¿ß¡¨
// 	UBorder* Border_Store;

// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
// 	UButton* Btn_Store;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UBorder* Border_Hotbar;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UProfilePanel* Profile;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UProfilePanel> wbp_profile;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UInventoryPanel* Inventory;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UInventoryPanel> wbp_inventory;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UContainerPanel* Container;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UContainerPanel> wbp_container;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UHotbarPanel* Hotbar;
	TSubclassOf<UHotbarPanel> wbp_hotbar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInventoryItemSlot* slotWidget;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UInventoryItemSlot> wbp_itemSlot;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UHotbarItemSlot* hotbarSlot;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UHotbarItemSlot> wbp_hotbarSlot;

// 	UPROPERTY(EditAnywhere,BlueprintReadWrite)
// 	UStorePanel* Store;
	
	UFUNCTION()
	void OnClickedQuitButton();
protected:
	
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
