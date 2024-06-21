

#include "InteractionTestActor.h"
#include "Components/StaticMeshComponent.h"

AInteractionTestActor::AInteractionTestActor()
{
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(mesh);
	
}

void AInteractionTestActor::BeginPlay()
{
	Super::BeginPlay();

	interactableData = instanceInteractableData;
	
}

void AInteractionTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractionTestActor::BeginFocus()
{
	if (mesh)
	{
		mesh->SetRenderCustomDepth(true);
	}
}

void AInteractionTestActor::EndFocus()
{
	if (mesh)
	{
		mesh->SetRenderCustomDepth(false);
	}
}

void AInteractionTestActor::BeginInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling BeginInteract override on interface test actor."));
}

void AInteractionTestActor::EndInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling EndInteract override on interface test actor."));

}

void AInteractionTestActor::Interact(class AVICTIMSCharacter* playerCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("Calling Interact override on interface test actor."));

}

