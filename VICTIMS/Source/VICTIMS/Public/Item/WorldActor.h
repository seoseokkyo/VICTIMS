
#pragma once

#include "CoreMinimal.h"
#include "UsableActor.h"
#include "FSlotStructure.h"
#include "VictimsGameInstance.h"
#include "WorldActor.generated.h"

class UDataTable;
UCLASS()
class VICTIMS_API AWorldActor : public AUsableActor
{
	GENERATED_BODY()
public:
	AWorldActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 Amount;

	UPROPERTY(Replicated)
	UStaticMesh* WorldMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool StartWithPhysicsEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* ItemDB;

	UFUNCTION()
	bool LoadItemFromList();

	UFUNCTION(Server, Reliable)
	void Server_InitializeItemData();
	
	virtual bool OnActorUsed_Implementation(APlayerController* Controller) override;

protected:
	UPROPERTY()
	FSlotStructure InventoryItem;
};
