
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class AVICTIMSCharacter;

UCLASS()
class VICTIMS_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY()
	AVICTIMSCharacter* playerCharacter;

protected:

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
};
