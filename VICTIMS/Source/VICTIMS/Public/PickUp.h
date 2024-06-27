
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUp.generated.h"

UCLASS()
class VICTIMS_API APickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	APickUp();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
