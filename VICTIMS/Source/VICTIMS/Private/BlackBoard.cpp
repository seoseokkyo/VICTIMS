// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackBoard.h"
#include "Camera/CameraComponent.h"
#include "AVICTIMSPlayerController.h"
#include "VICTIMSCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABlackBoard::ABlackBoard()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	ViewCamera->SetupAttachment(RootComponent); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation

	ViewCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	ViewCamera->SetRelativeLocation(FVector(45.0f, 150.0f, -40.0f));
	ViewCamera->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));



	IsUsable = true;
	Action = FText::FromString("Travel");
	Name = FText::FromString("Target Place");
}

// Called when the game starts or when spawned
void ABlackBoard::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABlackBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ABlackBoard::OnActorUsed_Implementation(APlayerController* Controller)
{
	if (bUsingNow)
	{
		return false;
	}

	useingPlayer = Cast<AVICTIMSPlayerController>(Controller);

	if (nullptr != useingPlayer)
	{
		if (ViewCamera)
		{
			auto charCheck = Cast<AVICTIMSCharacter>(useingPlayer->GetPawn());

			//안먹히네..
			//일단 투명화는 나중에
			//charCheck->GetMesh()->SetHiddenInGame(true, false);
			//charCheck->GetMesh()->SetVisibility(false, false);

			//charCheck->GetMesh()->SetRenderCustomDepth(true);
			//charCheck->GetMesh()->SetCustomDepthStencilValue(1);

			//if (PlayerMaskMaterial)
			//{
			//	charCheck->GetMesh()->SetOverlayMaterial(PlayerMaskMaterial);
			//	charCheck->GetMesh()->SetHiddenInGame(true, false);

			//	for (auto findComp : charCheck->GetComponents())
			//	{
			//		if (findComp->ComponentTags.Contains(FName("RTG_UEFN_to_UE4_Mannequin")))
			//		{
			//			if (auto skeletalCheck = Cast<USkeletalMeshComponent>(findComp))
			//			{
			//				// 이것도 안먹힘
			//				skeletalCheck->SetOverlayMaterial(PlayerMaskMaterial);

			//				UE_LOG(LogTemp, Warning, TEXT("RTG_UEFN_to_UE4_Mannequin Found"));

			//				break;
			//			}
			//		}
			//	}
			//}
			//else
			//{

			//}

			//PlayerController->EnableUIMode();			
			//PlayerController->SetShowMouseCursor(true);
			//PlayerController->bEnableClickEvents = true;
			//PlayerController->bEnableMouseOverEvents = true;

			float blendTime = 0.4f;

			if (useingPlayer->IsLocalController())
			{
				useingPlayer->SetViewTargetWithBlend(this, blendTime, VTBlend_Linear);

				if (1)
				{
					FTimerHandle timerHnd;
					GetWorldTimerManager().SetTimer(timerHnd, [&, charCheck]() {

						useingPlayer->bUseUIMode = true;
						useingPlayer->EnableUIMode();
						useingPlayer->SetShowMouseCursor(true);
						useingPlayer->bEnableClickEvents = true;
						useingPlayer->bEnableMouseOverEvents = true;

						}, blendTime + 0.3f, false);
				}
				else
				{
					useingPlayer->bUseUIMode = true;
					useingPlayer->EnableUIMode();
					useingPlayer->SetShowMouseCursor(true);
					useingPlayer->bEnableClickEvents = true;
					useingPlayer->bEnableMouseOverEvents = true;
				}
			}

			return false;
		}

		return true;
	}

	return false;
}

void ABlackBoard::ServerRPC_TravelRequest_Implementation(AVICTIMSPlayerController* Controller)
{
	MultiRPC_TravelRequest(Controller);
}

void ABlackBoard::MultiRPC_TravelRequest_Implementation(AVICTIMSPlayerController* Controller)
{
	this->EnableInput(Controller);

	auto charCheck = Controller->GetPawn();

	if (charCheck)
	{
		charCheck->DisableInput(Controller);
		charCheck->SetActorLocation(GetActorRightVector() * 300 + GetActorLocation());
	}

	float blendTime = 0.4f;

	Controller->SetViewTargetWithBlend(this, blendTime, VTBlend_Linear);

	FTimerHandle timerHnd;

	GetWorldTimerManager().SetTimer(timerHnd, [&]() {

		Controller->bUseUIMode = true;
		Controller->EnableUIMode();
		Controller->SetShowMouseCursor(true);
		Controller->bEnableClickEvents = true;
		Controller->bEnableMouseOverEvents = true;

		}, blendTime + 0.3f, false);
}

void ABlackBoard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlackBoard, bUsingNow);
}
