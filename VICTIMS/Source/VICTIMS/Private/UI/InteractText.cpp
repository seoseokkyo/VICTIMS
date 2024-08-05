

#include "InteractText.h"
#include "Components/TextBlock.h"


void UInteractText::ShowInteractText(const bool& bItem, const bool& bHousing, const bool& bHousingDel)
{
	SetVisibility(ESlateVisibility::Visible);

	if (bItem && !bHousing && !bHousingDel)	//������
	{
		InteractText->SetText(FText::FromString("Press F"));
	}
	if (!bItem && bHousing && !bHousingDel)	// ������ġ
	{
		InteractText->SetText(FText::FromString("Press B"));
	}
	if (!bItem && !bHousing && bHousingDel)	// �����ݱ�
	{
		InteractText->SetText(FText::FromString("Press Del"));
	}
}

void UInteractText::HideInteractText()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UInteractText::NativeConstruct()
{
	Super::NativeConstruct();
}