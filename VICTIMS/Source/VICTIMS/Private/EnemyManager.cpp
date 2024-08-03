// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"
#include "kismet/GameplayStatics.h"
#include "NormalZombie.h"


// Sets default values
AEnemyManager::AEnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();
	
    SetOwner(GetWorld()->GetFirstPlayerController());

    if (HasAuthority())
    {
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANormalZombie::StaticClass(), FoundActors);

        for (AActor* Actor : FoundActors)
        {
            enemys.Add({ Cast<ANormalZombie>(Actor), Actor->GetActorTransform(), 0.0f});
        }
    }
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (GetOwner() == nullptr)
    {
        SetOwner(GetWorld()->GetFirstPlayerController());
    }

    if (HasAuthority())
    {
        for (auto& enemy : enemys)
        {
            if (enemy.enemy->bDead == true)
            {
                enemy.dieTimer += DeltaTime;

                if (enemy.dieTimer > 20)
                {
                    enemy.dieTimer = 0.0f;

                    enemy.enemy->Controller->Destroy();
                    enemy.enemy->Destroy();

                    FActorSpawnParameters param;
                    param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

                    enemy.enemy = GetWorld()->SpawnActor<ANormalZombie>(bp_Enemy, enemy.originPos, param);
                    enemy.enemy->SpawnDefaultController();
                }
            }
        }
    }
}
