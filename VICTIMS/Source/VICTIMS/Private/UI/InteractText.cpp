

#include "InteractText.h"
#include "Components/TextBlock.h"


void UInteractText::ShowInteractText(const bool& bItem, const bool& bHousing, const bool& bHousingDel)
{
	InteractText->SetText(FText::FromString("Press F"));

	//bool bItemCheck = true;
	//bool bHousingCheck = false;
	//bool bHousingDelChecl = false;

	SetVisibility(ESlateVisibility::Visible);

	//if (bItem && !bHousing && !bHousingDel)	//아이템
	//{
	//	InteractText->SetText(FText::FromString("Press F"));
	//}
	//if (!bItem && bHousing && !bHousingDel)	// 가구배치
	//{
	//	InteractText->SetText(FText::FromString("Press B"));
	//}
	//if (!bItem && !bHousing && bHousingDel)	// 가구줍기
	//{
	//	InteractText->SetText(FText::FromString("Press Del"));
	//}
}

void UInteractText::HideInteractText()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UInteractText::NativeConstruct()
{
	Super::NativeConstruct();
}