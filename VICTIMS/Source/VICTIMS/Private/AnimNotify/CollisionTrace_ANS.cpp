// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionTrace_ANS.h"
#include "CombatComponent.h"
#include "BaseWeapon.h"
#include "CollisionComponent.h"
#include "Player/VICTIMSCharacter.h"


void UCollisionTrace_ANS::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp != nullptr)
	{
		AActor* owenrActor = MeshComp->GetOwner();
		if (owenrActor != nullptr)
		{

			/*auto comps = owenrActor->GetComponentsByTag(UCombatComponent::StaticClass(), FName(TEXT("CombatComp")));
			for (auto Comp : comps)
			{
				auto combatComp = Cast<UCombatComponent>(Comp);
				if (combatComp != nullptr)
				{
					ABaseWeapon* mainWeapon = combatComp->GetMainWeapon();

					if (IsValid(mainWeapon))
					{
						if (mainWeapon->collisionComponent != nullptr)
						{
							mainWeapon->collisionComponent->SetEnableCollision(true);	// Enable
						}
					}
				}
			}
			*/
			//myPlayer에 달려있는 애를 메인 웨폰으로 찾아서, myPlayer에 Weapon 태그 달린 HandSkeletal을 mainWeapon으로 설정하고, 주인이 콜리전컴프를 가지고 있다면 켜줌. 콜리전 컴프는 알아서 스타트 지점과 엔드 지점을 찾아서 콜리전 검사 해 
			auto comps = owenrActor->GetComponentsByTag(USkeletalMeshComponent::StaticClass(),(TEXT("Weapon")));
			for (auto Comp : comps)
			{
				auto mainWeapon = Cast<USkeletalMeshComponent>(Comp);
				if (mainWeapon != nullptr)
				{

					AVICTIMSCharacter*myPlayer= Cast<AVICTIMSCharacter>(mainWeapon->GetOwner());
					if (IsValid(myPlayer))
					{
						if (myPlayer->collisionComponent != nullptr)
						{
							myPlayer->collisionComponent->SetEnableCollision(true);	// Enable
						}
					}
				}
			}

		}
	}
}

void UCollisionTrace_ANS::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr)
	{
		AActor* owenrActor = MeshComp->GetOwner();
		if (owenrActor != nullptr)
		{
			auto comps = owenrActor->GetComponentsByTag(USkeletalMeshComponent::StaticClass(), (TEXT("Weapon")));
			for (auto Comp : comps)
			{
				auto mainWeapon = Cast<USkeletalMeshComponent>(Comp);
				if (mainWeapon != nullptr)
				{
					AVICTIMSCharacter* myPlayer = Cast<AVICTIMSCharacter>(mainWeapon->GetOwner());
					if (IsValid(myPlayer))
					{
						if (myPlayer->collisionComponent != nullptr)
						{
							myPlayer->collisionComponent->SetEnableCollision(false);	// Enable
						}
					}
				}
			}
		}
	}
}

