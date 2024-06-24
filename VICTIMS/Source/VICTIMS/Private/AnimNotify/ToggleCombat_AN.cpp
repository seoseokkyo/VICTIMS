// Fill out your copyright notice in the Description page of Project Settings.


#include "ToggleCombat_AN.h"
#include "CombatComponent.h"
#include "BaseWeapon.h"
#include "CharacterAnimInstance.h"
#include "GameFramework/Character.h"

void UToggleCombat_AN::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr)
	{
		AActor* owenrActor = MeshComp->GetOwner();
		if (owenrActor != nullptr)
		{
			UCombatComponent* combatComp = owenrActor->GetComponentByClass<UCombatComponent>();

			if (combatComp != nullptr)
			{
				ABaseWeapon* mainWeapon = combatComp->GetMainWeapon();

				if (IsValid(mainWeapon))
				{
					ACharacter* player = Cast<ACharacter>(MeshComp->GetOwner());
					if (player != nullptr)
					{
						UCharacterAnimInstance* charAnim = Cast<UCharacterAnimInstance>(MeshComp->GetAnimInstance());
						if (charAnim != nullptr)
						{
							FName selectedSocketName = combatComp->bCombatEnable ? mainWeapon->handSocketName : mainWeapon->AttachSocketName;

							mainWeapon->AttachActor(selectedSocketName);

							combatComp->SetCombatEnabled(combatComp->bCombatEnable);
						}
					}
				}
			}
		}
	}
}

