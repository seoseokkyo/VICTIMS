
#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "EquipmentComponent.generated.h"

class AVICTIMSCharacter;
UCLASS(ClassGroup = (Custom), meta=(BlueprintSpawnableComponent))
class VICTIMS_API UEquipmentComponent : public UInventoryComponent
{
	GENERATED_BODY()
	
public: 
	UEquipmentComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void ClearInventoryItem(uint8 InventorySlot) override;
	virtual void SetInventoryItem(uint8 InventorySlot, FSlotStructure& Item) override;

	UFUNCTION()
	virtual void UpdateEquippedMeshes(uint8 InventorySlot);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Equipment Character Reference", Category = "Default", OverrideNativeName = "EquipmentCharacterReference"))
	AVICTIMSCharacter* EquipmentCharacterReference;

protected:
	
	virtual void BeginPlay() override;
};
