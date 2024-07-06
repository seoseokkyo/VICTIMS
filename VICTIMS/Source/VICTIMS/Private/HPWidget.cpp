#include "HPWidget.h"
#include "VICTIMSCharacter.h"
#include "StateComponent.h"
#include "Components/ProgressBar.h"

void UHPWidget::NativeConstruct()
{
	Super::NativeConstruct();

	auto characterCheck = Cast<AVICTIMSCharacter>(GetOwningPlayerPawn());
	if (characterCheck)
	{
		characterCheck->stateComp->OnCharacterRunningStatChanged.AddDynamic(this, &UHPWidget::OnUpdateCharacterStat);
	}	
}

void UHPWidget::OnUpdateCharacterStat(FCharacterRunningStat characterRunningStat)
{
	UpdateHPBar(characterRunningStat.currentHP, characterRunningStat.MaxHP);
	//UpdateSPBar(characterRunningStat.currentSP, characterRunningStat.MaxSP);
}

void UHPWidget::UpdateHPBar(float curHP, float maxHP)
{
	HP = curHP / maxHP;

	hpBar->SetPercent(curHP / maxHP);
}
