

#include "UsableActor.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "InteractText.h"
#include "InteractiveText_Entry.h"

AUsableActor::AUsableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	SetRootComponent(Scene);
	StaticMesh->AttachToComponent(Scene, FAttachmentTransformRules::KeepRelativeTransform);

	Name = FText::FromString("NULL");
	Action = FText::FromString("Use");
	IsUsable = true;
	WasUsed = false;

	InteractUserWidget = nullptr;
}

void AUsableActor::BeginPlay()
{
	Super::BeginPlay();
}

void AUsableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUsableActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUsableActor, Name);
	DOREPLIFETIME(AUsableActor, IsUsable);
	DOREPLIFETIME(AUsableActor, WasUsed);
}

bool AUsableActor::OnActorUsed_Implementation(APlayerController* Controller)
{
	if (HasAuthority()) {
		if (WasUsed)
		{
			WasUsed = false;
		}
		else {
			WasUsed = true;
		}
		OnRep_WasUsed();

		return true;
	}
	return false;
}

bool AUsableActor::EndOutlineFocus_Implementation()
{
	StaticMesh->SetRenderCustomDepth(false);
	StaticMesh->SetCustomDepthStencilValue(0);

	return true;
}

bool AUsableActor::BeginOutlineFocus_Implementation()
{
	StaticMesh->SetRenderCustomDepth(true);
	StaticMesh->SetCustomDepthStencilValue(2);

	return true;
}

bool AUsableActor::GetIsActorUsable_Implementation()
{
 	return IsUsable;
}

FText AUsableActor::GetUseActionText_Implementation()
{
	FFormatNamedArguments Args;
	Args.Add("Action", Action);
	Args.Add("Name", Name);

	FText FormattedText = FText::Format(
		NSLOCTEXT("MyNamespace", "MyKey", "{Action} {Name}"),
		Args
	);
	return FormattedText;
}
bool AUsableActor::OnWasUsed()
{
	return true;
}

void AUsableActor::OnRep_WasUsed()
{
	OnWasUsed();
}

void AUsableActor::SetInteractText(FText Text)
{
	if (UInteractiveText_Entry* Widget = Cast<UInteractiveText_Entry>(InteractUserWidget))
	{
		FString LocalString = Text.ToString();
		FName LocalName = *LocalString;

		Widget->SetEntryText(LocalName);
	}
}

void AUsableActor::SetScreenPosition(FVector2D ScreenPosition)
{
	InteractUserWidget->SetPositionInViewport(ScreenPosition);
}

