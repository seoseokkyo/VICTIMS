

#include "ProfileLayout.h"
#include "Components/UniformGridSlot.h"
#include "Components/UniformGridPanel.h"
#include "Internationalization/StringTableRegistry.h"
#include "AVICTIMSPlayerController.h"
#include "Attribute_Entry.h"
#include "FWidgetsLayoutBP.h"
#include "VictimsGameInstance.h"
#include "Components/ListView.h"
#include "EAttributes.h"
#include "MyHUD.h"

void UProfileLayout::NativeConstruct()
{
	Super::NativeConstruct();

	FText Text = FText::FromString("Profile");
	Super::SetTitleToWindow(Text);

	WindowLayout = ELayout::Equipment;
}

void UProfileLayout::UpdatePlayerStatsUI(int32 StregthValue)
{
	Strength->SetText(FText::AsNumber(StregthValue));
}
