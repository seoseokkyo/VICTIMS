
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"
#include "InteractionTestActor.generated.h"

struct FInteractableData;

UCLASS()
class VICTIMS_API AInteractionTestActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	AInteractionTestActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Test Actor")
	UStaticMeshComponent* mesh;

	UPROPERTY(EditInstanceOnly, Category = "Test Actor")
	FInteractableData instanceInteractableData;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void BeginInteract() override;
	virtual void EndInteract() override;
	virtual void Interact(class AVICTIMSCharacter* playerCharacter) override;

};
