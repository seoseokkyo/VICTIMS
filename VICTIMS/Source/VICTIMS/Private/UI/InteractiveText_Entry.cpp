

#include "InteractiveText_Entry.h"
#include "AVICTIMSPlayerController.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Internationalization/StringTableRegistry.h"
#include "FItemStructure.h"
#include "VictimsGameInstance.h"


void UInteractiveText_Entry::NativeConstruct()
{
	Super::NativeConstruct();

	SelectedImage->SetOpacity(0.f);

	PC = Cast<AVICTIMSPlayerController>(GetOwningPlayer());
	if (!IsValid(PC))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Controller Not Valid")));
	}

	GameInstance = Cast<UVictimsGameInstance>(GetGameInstance());
	if (!IsValid(GameInstance))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("GameInstance Not Valid")));
	}
}

void UInteractiveText_Entry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	SelectedImage->SetOpacity(0.f);
	NameLabel->SetText(FText::FromName("Default"));

	if (UInteractiveText_Entry* Entry = Cast<UInteractiveText_Entry>(ListItemObject))
	{
		if (IsValid(Entry) && IsValid(PC))
		{
			const UDataTable* DataTable = PC->GetItemDB();
			if (IsValid(DataTable))
			{
				FItemStructure* NewItemData = DataTable->FindRow<FItemStructure>(FName(Entry->ID), "", true);
				if (NewItemData)
				{
					IconImage->SetBrushFromTexture(NewItemData->Icon);

					FString ItemName = NewItemData->ID.ToString();
					FText ItemNameText = FText::FromString(ItemName);

					NameLabel->SetText(ItemNameText);

				}
				else {
					if (IconTexture)
					{
						IconImage->SetBrushFromTexture(IconTexture);
					}

					NameLabel->SetJustification(ETextJustify::Center);

					FString String = Entry->TextFromUsableActor.ToString();

					NameLabel->SetText(Entry->TextFromUsableActor);

					SelectedImage->SetBrushTintColor(FSlateColor(FLinearColor{ 1,1,1,0 }));
					SelectedImage->SetOpacity(0.f);
				}
			}
		}
	}
}

void UInteractiveText_Entry::NativeOnEntryReleased()
{
	IUserObjectListEntry::NativeOnEntryReleased();
	const FLinearColor LinearColor = { 1,1,1,1 };
}

void UInteractiveText_Entry::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);

	FLinearColor LinearColor;

	if (bIsSelected)
	{
		LinearColor = { 0.75,0.25,0,1 };

		SelectedImage->SetOpacity(1.f);
	}
	else {
		LinearColor = { 1,1,1,0 };

		SelectedImage->SetOpacity(0.f);
	}
}

void UInteractiveText_Entry::SetEntryText(const FName& Name)
{
	ID = Name;
}

FName UInteractiveText_Entry::GetEntryText()
{
	return ID;
}

void UInteractiveText_Entry::SetNameLabelText(FText Text)
{
	TextFromUsableActor = Text;
}
