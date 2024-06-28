
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HUDInterface.h"
#include "AVICTIMSPlayerController.generated.h"

class UDatatable;
class UItembase;
class AVICTIMSCharacter;
class UInventoryComponent;

UCLASS()
class VICTIMS_API AVICTIMSPlayerController : public APlayerController, public IHUDInterface
{
	GENERATED_BODY()

public: 

	AVICTIMSPlayerController();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(Category="Inventory", OverrideNativeName="InventoryComponent"))
	UInventoryComponent* inventoryComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)	
	AVICTIMSCharacter* victimsPlayer;

	UPROPERTY()
	TObjectPtr<UItemBase> item;


	// ========================================================================================================
	// 인터페이스 
	// ========================================================================================================

	virtual void UseInventoryItem_Implementation(const TObjectPtr<UItemBase> _item) override;
};
