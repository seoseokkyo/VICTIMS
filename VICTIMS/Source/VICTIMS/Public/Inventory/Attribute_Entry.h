
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/TextBlock.h"
#include "Attribute_Entry.generated.h"

UCLASS()
class VICTIMS_API UAttribute_Entry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SetAttributeText(const FText& Text);

	// IUserObjectListEntry
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	// IUserObjectListEntry

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AttributeTextBlock;

	UPROPERTY()
	FText AttributeText;

	UPROPERTY()
	uint8 ID;

	UFUNCTION()
	void SetID(uint8& Index);
};