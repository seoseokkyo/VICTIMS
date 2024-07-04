#include "FootStepNotify.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <VICTIMSCharacter.h>
#include "Perception/AISenseConfig_Hearing.h"

void UFootStepNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp)
	{
		if (MeshComp->GetOwner())
		{
			auto characterCheck = Cast<AVICTIMSCharacter>(MeshComp->GetOwner());
			if (characterCheck)
			{
				float currentSpeed = characterCheck->GetCharacterMovement()->Velocity.Size() * 0.1;
				UAISense_Hearing::ReportNoiseEvent(GetWorld(), MeshComp->GetComponentLocation(), currentSpeed, MeshComp->GetOwner(), 1000);
			}
			else
			{
				UAISense_Hearing::ReportNoiseEvent(GetWorld(), MeshComp->GetComponentLocation(), 10, MeshComp->GetOwner(), 1000);
			}
		}
	}
}
