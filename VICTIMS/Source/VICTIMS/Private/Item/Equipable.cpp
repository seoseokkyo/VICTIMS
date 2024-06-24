// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipable.h"
#include <Components/SkeletalMeshComponent.h>
#include <Components/StaticMeshComponent.h>
#include "Components/PrimitiveComponent.h"
#include <GameFramework/Character.h>

// Sets default values
AEquipable::AEquipable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(rootComp);

	ItemSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemSkeletalMesh"));
	ItemSkeletalMesh->SetupAttachment(rootComp);

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMesh"));
	ItemStaticMesh->SetupAttachment(rootComp);
}

// Called when the game starts or when spawned
void AEquipable::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AEquipable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UPrimitiveComponent* AEquipable::GetItemMesh()
{
	if (IsValid(ItemStaticMesh->GetStaticMesh()))
	{
		return ItemStaticMesh;
	}
	else
	{
		return ItemSkeletalMesh;
	}
}

void AEquipable::AttachActor(FName SocketName)
{
	ACharacter* ownerCharacter = Cast<ACharacter>(GetOwner());

	if (ownerCharacter != nullptr)
	{
		FAttachmentTransformRules attachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);

		this->AttachToComponent(ownerCharacter->GetMesh(), attachRules, SocketName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ownerCharacter Cast Failed"));
	}
}

void AEquipable::OnEquipped()
{
	bIsEquipped = true;

	if (AttachSocketName != "")
	{
		AttachActor(AttachSocketName);
	}
}

void AEquipable::OnUnEquipped()
{
	if (bIsEquipped)
	{
		bIsEquipped = false;
	}
}

void AEquipable::SetIsEquipped(bool equipped)
{
	bIsEquipped = equipped;
}

bool AEquipable::EquippedCheck()
{
	return bIsEquipped;
}

