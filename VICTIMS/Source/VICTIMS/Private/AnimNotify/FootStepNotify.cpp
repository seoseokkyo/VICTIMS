#include "FootStepNotify.h"
#include "Perception/AISenseConfig_Hearing.h"

void UFootStepNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp)
	{
		if (MeshComp->GetOwner())
		{
			UAISense_Hearing::ReportNoiseEvent(GetWorld(), MeshComp->GetComponentLocation(), 10, MeshComp->GetOwner(), 1000);
		}
	}
}
