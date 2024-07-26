
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InteractiveText_Entry.generated.h"

class UTextBlock;
class UImage;
class UBorder;
UCLASS()
class VICTIMS_API UInteractiveText_Entry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SetEntryText(const FName& Name);

	UFUNCTION()
	FName GetEntryText();

	void SetNameLabelText(FText Text);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* NameLabel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* SelectedImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* IconTexture;

protected:

	UFUNCTION()
	virtual void NativeConstruct() override;
	
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	virtual void NativeOnEntryReleased() override;

	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* IconImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	FName ID;

	UPROPERTY()
	class AVICTIMSPlayerController* PC;

	UPROPERTY()
	class UVictimsGameInstance* GameInstance;

	UPROPERTY()
	FText TextFromUsableActor;
};
