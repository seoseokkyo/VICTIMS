// Fill out your copyright notice in the Description page of Project Settings.


#include "HousingComponent.h"
#include "VICTIMSCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/DataTableFunctionLibrary.h>
#include "Engine/DataTable.h"
#include "HousingInterface.h"
#include "Net/UnrealNetwork.h"

#include "Item/FItemStructure.h"
#include "AVICTIMSPlayerController.h"
#include "Item/FSlotStructure.h"
#include "InventoryManagerComponent.h"
#include "InventoryComponent.h"
#include "EquipmentComponent.h"

UHousingComponent::UHousingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	IsBuildModeOn = false;
	CanBuild = false;
	BuildID = 0;

	IsMoving = false;
	SetIsReplicatedByDefault(true);
}

void UHousingComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner)
	{
		AVICTIMSCharacter* HousingTestCharacter = Cast<AVICTIMSCharacter>(Owner);
		if (HousingTestCharacter)
		{
			PlayerRef = HousingTestCharacter;
			Camera = HousingTestCharacter->FindComponentByClass<UCameraComponent>(); // ī�޶� ������Ʈ �ʱ�ȭ
		}
	}

	if (DB_Housing)
	{

		// ������ ���̺��� �� �̸� ��������
		/*TArray<FName> */RowNames = DB_Housing->GetRowNames();

		UE_LOG(LogTemp, Warning, TEXT("Number of rows in the data table: %d"), RowNames.Num());


		for (const FName& RowName : RowNames)
		{
			// �� ������ ��������
			FBuildablesStructs* RowData = DB_Housing->FindRow<FBuildablesStructs>(RowName, TEXT(""));

			// �� ������ Buildables �迭�� �߰�
			if (RowData)
			{
				UE_LOG(LogTemp, Warning, TEXT("Found row: %s"), *RowName.ToString());
				Buildables.Add(*RowData);
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Number of buildables added: %d"), Buildables.Num());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DB_Housing is null"));
	}
	if (DB_Item)
	{
		ItemRowNames = DB_Item->GetRowNames();
	}
}

void UHousingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	OnRep_IsBuildModeOn();
}


void UHousingComponent::SpawnPreviewMesh()
{
	if (!PlayerRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerRef is null"));
		return;
	}

	// BuildID�� ��ȿ���� Ȯ��
	if (BuildID < 0 || BuildID >= Buildables.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid BuildID"));
		return;
	}

	// Buildables �迭���� �ش� ��Ҹ� ������
	const FBuildablesStructs& Buildable = Buildables[BuildID];

	PreviewMesh = NewObject<UStaticMeshComponent>(PlayerRef);

	if (PreviewMesh)
	{
		PreviewMesh->AttachToComponent(PlayerRef->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		PreviewMesh->SetRelativeTransform(BuildTransform);
		PreviewMesh->RegisterComponent();

		// Buildables �迭���� ������ Mesh�� ����
		if (Buildable.Mesh)
		{
			PreviewMesh->SetStaticMesh(Buildable.Mesh);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Buildable Mesh is null"));
		}

		// �ݸ��� ��Ȱ��ȭ
		PreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		GiveBuildColor();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create Static Mesh Component"));
	}
}

void UHousingComponent::GiveBuildColor()
{
	if (!PreviewMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("PreviewMesh is null"));
		return;
	}

	//CanBuild = false;

	bool bCanBuild = CanPlacePreviewMesh();
	CanBuild = bCanBuild;

	UMaterialInterface* MaterialToApply = bCanBuild ? GreenMaterial : RedMaterial;

	if (MaterialToApply)
	{
		// ��Ƽ������ ���� ��ü�� �����Ŵϱ� �װ� �� ����̵� �������̵� �ٲ����ϴϱ� � ��Ƽ���� �ִ��� ���� / GetNumMaterials()�� PreviewMesh�� ��Ƽ���� ���� ������
		int32 NumMaterials = PreviewMesh->GetNumMaterials();
		for (int32 Index = 0; Index < NumMaterials; ++Index)
		{
			// ������ �ε����� �ִ� ��Ƽ������ ���ο� ��Ƽ������ MaterialToApply�� ����
			PreviewMesh->SetMaterial(Index, MaterialToApply);
		}

		//CanBuild = true;

		PreviewMesh->SetWorldTransform(BuildTransform);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Material to apply is null"));
	}
}

void UHousingComponent::BuildCycle()
{
	if (!Camera)
	{
		UE_LOG(LogTemp, Warning, TEXT("Camera is null"));
		return;
	}

	// BuildID�� ��ȿ���� Ȯ��
	if (BuildID < 0 || BuildID >= Buildables.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid BuildID"));
		return;
	}

	const FBuildablesStructs& Buildable = Buildables[BuildID];

	FVector CameraLocation = Camera->GetComponentLocation();
	FRotator CameraRotation = Camera->GetComponentRotation();
	FVector StartLocation = CameraLocation;
	FVector EndLocation = StartLocation + (CameraRotation.Vector() * 1500.0f);

	FHitResult HitResult;
	FHitResult HitResult2;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	ECollisionChannel temp = UEngineTypes::ConvertToCollisionChannel(Buildable.TraceChannel);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
	bool bHit2 = GetWorld()->LineTraceSingleByChannel(HitResult2, StartLocation, EndLocation, ECC_GameTraceChannel6, Params);

	if (bHit2)
	{
		AActor* HitActor = HitResult2.GetActor();
		if (HitActor->ActorHasTag(FName("Allow")))
		{
			if (bHit)
			{
				FVector ImpactPoint = HitResult.ImpactPoint;

				BuildTransform.SetLocation(ImpactPoint + FVector(0, 0, 10));
				if (IsValid(PreviewMesh))
				{
					GiveBuildColor();
				}
				else
				{
					SpawnPreviewMesh();
				}
			}
			else
			{
				BuildTransform.SetLocation(EndLocation);
				if (IsValid(PreviewMesh))
				{
					GiveBuildColor();
					//BuildDelay();
				}
				else
				{
					SpawnPreviewMesh();
					//BuildDelay();
				}
			}
		}
	}
	
	if (IsBuildModeOn)
	{
		GetWorld()->GetTimerManager().SetTimer(BuildCycleTimerHandle, this, &UHousingComponent::BuildDelay, 0.1f, false);
	}
	else
	{
		IsBuildModeOn = false;
		CanBuild = false;
		if (IsValid(PreviewMesh))
		{
			PreviewMesh->DestroyComponent();
			PreviewMesh = nullptr;
		}
	}
}

void UHousingComponent::StartBuildCycle()
{
	IsBuildModeOn = true;
	BuildCycle();
}

void UHousingComponent::StopBuildCycle()
{
	IsBuildModeOn = false;
	CanBuild = false;
	GetWorld()->GetTimerManager().ClearTimer(BuildCycleTimerHandle);
}

void UHousingComponent::BuildDelay()
{
	//if (IsBuildModeOn)
	//{
	//    FTimerHandle handle;
	//    GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([&]() {
	//        BuildCycle();
	//    }), 0.1, true);
	//}
	//else
	//{
	//    IsBuildModeOn = false;
	//    CanBuild = false;
	//    if (IsValid(PreviewMesh))
	//    {
	//        PlayerRef->DestroyComponent(PreviewMesh);
	//        PreviewMesh = nullptr;
	//    }
	//}
	if (IsBuildModeOn)
	{
		BuildCycle();
	}
}

void UHousingComponent::StopBuildMode()
{
	IsBuildModeOn = false;
	CanBuild = false;

	MoveableActor = nullptr;
	IsMoving = false;

	if (IsValid(PreviewMesh))
	{
		PlayerRef->DestroyComponent(PreviewMesh);
		PreviewMesh = nullptr;
	}

	ClientRPC_StopBuildMode();
}

void UHousingComponent::ClientRPC_StopBuildMode_Implementation()
{
	IsBuildModeOn = false;
	CanBuild = false;

	MoveableActor = nullptr;
	IsMoving = false;

	//if (IsValid(PreviewMesh))
	//{
	//	PlayerRef->DestroyComponent(PreviewMesh);
	//	PreviewMesh = nullptr;
	//}
}

void UHousingComponent::LaunchBuildMode(FName ItemID)
{
	// BuildID �α� ���
	//UE_LOG(LogTemp, Warning, TEXT("Launching Build Mode with BuildID: %d"), BuildID);

	//// BuildID�� ��ȿ���� �˻�
	//if (BuildID < 0 || BuildID >= Buildables.Num())
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Invalid BuildID: %d"), BuildID);
	//	// �⺻������ �����ϰų� ����ڿ��� �˸� ����
	//	BuildID = 0; // ���÷� �⺻���� 0���� ����
	//	return;
	//}

	//if (IsBuildModeOn)
	//{
	//	StopBuildMode();
	//}
	//else
	//{
	//	IsBuildModeOn = true;
	//	bool bBuildIDSet = false;
	//	// �κ��丮�� �����۰� �Ͽ�¡ ������ ���̺��� RowName�� ���Ͽ� BuildID ����
	//	for (const FName& ItemRowName : ItemRowNames)
	//	{
	//		FItemStructure* ItemRowData = DB_Item->FindRow<FItemStructure>(ItemRowName, TEXT(""));
	//		if (ItemRowData && ItemRowData->ItemType == EItemType::Furniture)
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("TESTInventory Item ID: %s"), *ItemRowData->ID.ToString());
	//			for (const FBuildablesStructs& Buildable : Buildables)
	//			{
	//				UE_LOG(LogTemp, Warning, TEXT("TESTHousing Item ID: %s"), *Buildable.ID.ToString());
	//				if (ItemRowData->ID == Buildable.ID)
	//				{
	//					BuildID = Buildables.IndexOfByKey(Buildable);
	//					UE_LOG(LogTemp, Warning, TEXT("TESTMatching BuildID found: %d"), BuildID);
	//					bBuildIDSet = true;
	//					break;
	//				}
	//			}
	//			if (bBuildIDSet)
	//			{
	//				break;
	//			}
	//		}
	//	}

	//	if (bBuildIDSet)
	//	{
	//		SpawnPreviewMesh();  // ������ �޽� ����
	//		BuildCycle();  // BuildCycle ����
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("No matching BuildID found in inventory"));
	//	}
	//}
	UE_LOG(LogTemp, Warning, TEXT("Launching Build Mode with Item ID: %s"), *ItemID.ToString());

	if (IsBuildModeOn)
	{
		StopBuildMode();
	}
	else
	{
		IsBuildModeOn = true;

		if (BuildID < 0 || BuildID >= Buildables.Num())
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid BuildID: %d"), BuildID);
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("Spawning Preview Mesh with BuildID: %d"), BuildID);
		SpawnPreviewMesh();
		BuildCycle();


	}
}


void UHousingComponent::ChangeMesh()
{
	// 	if (BuildID < 0 || BuildID >= Buildables.Num())
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("Invalid BuildID: %d"), BuildID);
	// 		// �⺻������ �����ϰų� ����ڿ��� �˸� ����
	// 		BuildID = 0; // ���÷� �⺻���� 0���� ����
	// 		return;
	// 	}

	// 	const FBuildablesStructs& Buildable = Buildables[BuildID];
	// 
	// 	if (PreviewMesh)
	// 	{
	// 		if (Buildable.Mesh)
	// 		{
	// 			PreviewMesh->SetStaticMesh(Buildable.Mesh);
	// 		}
	// 		else
	// 		{
	// 			UE_LOG(LogTemp, Warning, TEXT("PreviewMesh is null"));
	// 		}
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("PreviewMesh is null"));
	// 	}

	if (PreviewMesh)
	{
		if (Buildables.IsValidIndex(BuildID))
		{
			PreviewMesh->SetStaticMesh(Buildables[BuildID].Mesh);
			UE_LOG(LogTemp, Warning, TEXT("Mesh changed to: %s"), *Buildables[BuildID].Mesh->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid BuildID: %d"), BuildID);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PreviewMesh is null"));
	}
}

void UHousingComponent::SpawnBuild(bool Moving, AActor* Movable, const FTransform& Transform, const TArray<FBuildablesStructs>& DB, int32 ID)
{
	if (!Moving)
	{
		const FBuildablesStructs& Buildable = DB[ID];

		if (Buildable.Actor)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(Buildable.Actor, Transform, SpawnParams);
			// �׽�Ʈ
			//SpawnedActor->Tags.Add(FName(*PlayerRef->MyPlayerController->playerName));

			// UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("UHousingComponent :: SpawnBuild :: SpawnedActor : %p"), SpawnedActor));

			if (!SpawnedActor)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to spawn actor"));
			}
			// ������ ���Ͱ� HousingInterface�� �����ϰ� �ִ��� Ȯ��
			if (SpawnedActor->GetClass()->ImplementsInterface(UHousingInterface::StaticClass()))
			{
				// �������̽� �Լ� ȣ��
				IHousingInterface::Execute_SetMesh(SpawnedActor, Buildable.Mesh);
				IHousingInterface::Execute_SetBuildID(SpawnedActor, ID);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Spawned actor does not implement HousingInterface"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Buildable Actor is null"));
		}
		StopBuildMode();
	}
	else
	{
		if (Movable == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Movable Actor is null"));
		}
		else
		{
			// UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("UHousingComponent :: SpawnBuild->MoveObj :: Movable : %p"), Movable));

			Movable->SetActorTransform(Transform);
			StopBuildMode();
		}
	}
}

TArray<FBuildablesStructs> UHousingComponent::GetBuildablesArray() const
{
	return Buildables;
}

bool UHousingComponent::CanPlacePreviewMesh()
{
	if (!PreviewMesh)
	{
		return false;
	}

	FHitResult HitResult;
	FVector Start = PreviewMesh->GetComponentLocation();
	FVector End = Start;

	FCollisionQueryParams CollisionParams;
	// CollisionParams.AddIgnoredActor(PlayerRef);

	const FBuildablesStructs& Buildable = Buildables[BuildID];
	ECollisionChannel TraceChannel = UEngineTypes::ConvertToCollisionChannel(Buildable.TraceChannel);

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		TraceChannel,
		FCollisionShape::MakeSphere(10.0f),
		CollisionParams
	);

	// ����� ����� ���� ���� ���� �ٴ� �Ǵ� �� �浹 ���θ� Ȯ��
	if (!bHit || !HitResult.GetComponent())
	{
		return false;
	}

	// �ٴڿ� ��ġ ������ ��� �߰� Ȯ��
	if (TraceChannel == ECC_GameTraceChannel2)
	{
		if (!HitResult.GetComponent()->ComponentHasTag("Floor")) /*GetActor()->ActorHasTag("Floor"))*/
		{
			return false;
		}
	}

	// ���� ��ġ ������ ��� �߰� Ȯ��
	if (TraceChannel == ECC_GameTraceChannel3)
	{
		if (!HitResult.GetComponent()->ComponentHasTag("Wall"))
		{
			return false;
		}
	}

	if (HitResult.GetComponent()->IsA(ACharacter::StaticClass()))
	{
		return false;
	}

	return true;
}

void UHousingComponent::RemoveObject()
{
	Server_RemoveObject();
}

void UHousingComponent::MoveObject()
{
	Server_MoveObject();
}

void UHousingComponent::OnRep_IsBuildModeOn()
{
	if (IsBuildModeOn && !PreviewMesh)
	{
		SpawnPreviewMesh();
		UE_LOG(LogTemp, Warning, TEXT("Build mode enabled on client"));
	}
	else if (!IsBuildModeOn && PreviewMesh)
	{
		PreviewMesh->DestroyComponent();
		PreviewMesh = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Build mode disabled on client"));
	}
}


void UHousingComponent::Server_RemoveObject_Implementation()
{
	FVector CameraLocation = Camera->GetComponentLocation();
	FRotator CameraRotation = Camera->GetComponentRotation();
	FVector StartLocation = CameraLocation;
	FVector EndLocation = StartLocation + (CameraRotation.Vector() * 1500.0f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();

		if (HitActor->GetClass()->ImplementsInterface(UHousingInterface::StaticClass()))
		{
			int32 RemovedBuildID = IHousingInterface::Execute_ReturnBuildID(HitActor);
			FName RemovedItemID = Buildables[RemovedBuildID].ID;

			//UE_LOG(LogTemp, Warning, TEXT("%s will remove"), *RemovedItemID.ToString());
			HitActor->Destroy();

			AVICTIMSPlayerController* PlayerController = Cast<AVICTIMSPlayerController>(PlayerRef->GetController());
			if (PlayerController && PlayerController->InventoryManagerComponent)
			{
				FSlotStructure SlotStructure = PlayerController->InventoryManagerComponent->GetItemFromItemDB(RemovedItemID);
				SlotStructure.Amount = 1;

				bool bSuccess = false;
				UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(PlayerController->PlayerInventoryComponent);
				PlayerController->InventoryManagerComponent->TryToAddItemToInventory(InventoryComponent, SlotStructure, bSuccess);

				if (bSuccess)
				{
					// 					UE_LOG(LogTemp, Warning, TEXT("Item successfully added to inventory."));
					PlayerController->InventoryManagerComponent->Server_UpdateTooltips();
				}
				else
				{
					// 					UE_LOG(LogTemp, Warning, TEXT("Failed to add item to inventory."));
				}
			}
			else
			{
				// 				UE_LOG(LogTemp, Error, TEXT("Failed to cast PlayerInventoryComponent to UInventoryComponent."));
			}
		}
	}
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 1, 0, 1);
	if (bHit)
	{
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5, 12, FColor::Red, false, 1);
	}
}

void UHousingComponent::Multicast_RemoveObject_Implementation()
{
// 	RemoveObject();
}

void UHousingComponent::Server_MoveObject_Implementation()
{
	//if (!IsMoving || !MoveableActor)
	//{
	//	return;
	//}
	if (IsBuildModeOn)
	{
		return;
	}
	//IsBuildModeOn = false;
	IsMoving = true;

	FVector CameraLocation = Camera->GetComponentLocation();
	FRotator CameraRotation = Camera->GetComponentRotation();
	FVector StartLocation = CameraLocation;
	FVector EndLocation = StartLocation + (CameraRotation.Vector() * 1500.0f);

	Multicast_MoveObject(StartLocation, EndLocation);
}

void UHousingComponent::Multicast_MoveObject_Implementation(FVector StartLocation, FVector EndLocation)
{

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);

	if (bHit)
	{
		MoveableActor = HitResult.GetActor();

		// UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("UHousingComponent :: Multicast_MoveObject :: Movable : %p"), MoveableActor));

		if (MoveableActor->GetClass()->ImplementsInterface(UHousingInterface::StaticClass()))
		{
			// �������̽� �Լ� ȣ��
			BuildID = IHousingInterface::Execute_ReturnBuildID(MoveableActor);
			UE_LOG(LogTemp, Warning, TEXT("Build ID: %d"), BuildID);

			FName ItemID = Buildables[BuildID].ID;
			this->BuildID = BuildID;

			LaunchBuildMode(ItemID);


		}
		else
		{
			// ���⼭ movable�� �ƴ� ���Ͱ� Hit�Ȱ�� ó��
		}
	}
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 1, 0, 1);
	if (bHit)
	{
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5, 12, FColor::Red, false, 1);
	}
}

void UHousingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHousingComponent, IsBuildModeOn);
	DOREPLIFETIME(UHousingComponent, MoveableActor);
	DOREPLIFETIME(UHousingComponent, IsMoving);
}