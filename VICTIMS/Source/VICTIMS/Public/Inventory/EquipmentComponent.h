
#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "EquipmentComponent.generated.h"

class AVICTIMSCharacter;
UCLASS()
class VICTIMS_API UEquipmentComponent : public UInventoryComponent
{
	GENERATED_BODY()

public:	
	UEquipmentComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void ClearInventoryItem(uint8 InventorySlot) override;
	virtual void SetInventoryItem(uint8 InventorySlot, FSlotStructure& Item) override;

	UFUNCTION()
	virtual void UpdateEquippedMeshes(uint8 InventorySlot);

	UFUNCTION(Server, Reliable)
	void ServerRPC_UpdateEquippedMeshes(uint8 InventorySlot);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPC_UpdateEquippedMeshes(uint8 InventorySlot, FSlotStructure Slot, USkeletalMesh* NewMesh);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(DisplayName="Equipment Character Reference", Category="Default", OverrideNativeName="EquipmentCharacterReference"))
	AVICTIMSCharacter* EquipmentCharacterReference;

	UPROPERTY()
	USkeletalMeshComponent* ChestMesh;
	UPROPERTY()
	USkeletalMeshComponent* BottomMesh;
	UPROPERTY()
	USkeletalMeshComponent* FeetMesh;
	UPROPERTY()
	USkeletalMeshComponent* HeadMesh;

};
