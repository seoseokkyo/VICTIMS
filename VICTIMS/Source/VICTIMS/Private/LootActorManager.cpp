// Fill out your copyright notice in the Description page of Project Settings.


#include "LootActorManager.h"
#include "kismet/GameplayStatics.h"
#include "LootActor.h"

// Sets default values
ALootActorManager::ALootActorManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALootActorManager::BeginPlay()
{
	Super::BeginPlay();

    SetOwner(GetWorld()->GetFirstPlayerController());

    if (HasAuthority())
    {
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&](){

            TArray<AActor*> FoundActors;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALootActor::StaticClass(), FoundActors);

            lootActors.Reset();
            for (AActor* Actor : FoundActors)
            {
                lootActors.Add({ Cast<ALootActor>(Actor), 0.0f });
            }

        }, 8.0f, false);
    }
}

// Called every frame
void ALootActorManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (ownerCheckTimer > 5)
    {
        ownerCheckTimer = 0.0f;

        if (GetOwner() == nullptr)
        {
            SetOwner(GetWorld()->GetFirstPlayerController());
        }
    }
    else
    {
        ownerCheckTimer += DeltaTime;
    }

    if (HasAuthority())
    {
        for (auto& lootActor : lootActors)
        {
            if (lootActor.lootActor->GetIsActorUsable_Implementation() == false)
            {
                lootActor.emptyTimer += DeltaTime;

                if (lootActor.emptyTimer > 20)
                {
                    lootActor.emptyTimer = 0.0f;
                    
                    lootActor.lootActor->InitializeInventory();
                }
            }
        }
    }
}

