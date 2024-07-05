
#pragma once

#include "CoreMinimal.h"
#include "WindowLayout.h"
#include "FItemStructure.h"
#include "ProfileLayout.generated.h"

class UTextBlock;
class UUniformGridPanel;
class USlotLayout;
UCLASS()
class VICTIMS_API UProfileLayout : public UWindowLayout
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UUniformGridPanel* EquipmentGridPanel;
	
	UPROPERTY()
	TArray<USlotLayout*>EquipmentSlotsArray;

	UPROPERTY(meta =(BindWidget))
	UTextBlock* Strength;
	
	UFUNCTION()
	void UpdatePlayerStatsUI(int32 StrengthValue);
private:
	virtual void NativeConstruct() override;

};
