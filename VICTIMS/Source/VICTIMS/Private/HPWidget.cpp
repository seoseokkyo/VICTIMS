

#include "HPWidget.h"


void UHPWidget::NativeConstruct()
{
	Super::NativeConstruct();
}
void UHPWidget::UpdateHPBar(float curHP, float maxHP)
{
	HP = curHP / maxHP;
}
