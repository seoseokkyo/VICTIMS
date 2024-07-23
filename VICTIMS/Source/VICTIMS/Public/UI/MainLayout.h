
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainLayout.generated.h"


struct FSlotStructure;
UCLASS()
class VICTIMS_API UMainLayout : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UInventoryLayout* Inventory;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UProfileLayout* Profile;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UContainerLayout* Container;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UHotbar* Hotbar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta =(BindWidget))
	class UShopLayout* Shop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta =(BindWidget))
	class UInteractText* Interact;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta =(BindWidget))
	class USavedWidget* Saved; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UDropMoneyLayout* DropMoneyLayout;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UKillWidget* KillWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UCompassWedget* CompassWidget;

protected:
	UFUNCTION()
	virtual void NativeConstruct() override;
	
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};