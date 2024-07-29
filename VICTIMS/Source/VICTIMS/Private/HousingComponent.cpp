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
			Camera = HousingTestCharacter->FindComponentByClass<UCameraComponent>(); // 카메라 컴포넌트 초기화
		}
	}

	if (DB_Housing)
	{

		// 데이터 테이블에서 행 이름 가져오기
		/*TArray<FName> */RowNames = DB_Housing->GetRowNames();

		UE_LOG(LogTemp, Warning, TEXT("Number of rows in the data table: %d"), RowNames.Num());


		for (const FName& RowName : RowNames)
		{
			// 행 데이터 가져오기
			FBuildablesStructs* RowData = DB_Housing->FindRow<FBuildablesStructs>(RowName, TEXT(""));

			// 행 데이터 Buildables 배열에 추가
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

	// BuildID가 유효한지 확인
	if (BuildID < 0 || BuildID >= Buildables.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid BuildID"));
		return;
	}

	// Buildables 배열에서 해당 요소를 가져옴
	const FBuildablesStructs& Buildable = Buildables[BuildID];

	PreviewMesh = NewObject<UStaticMeshComponent>(PlayerRef);

	if (PreviewMesh)
	{
		PreviewMesh->AttachToComponent(PlayerRef->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		PreviewMesh->SetRelativeTransform(BuildTransform);
		PreviewMesh->RegisterComponent();

		// Buildables 배열에서 가져온 Mesh를 설정
		if (Buildable.Mesh)
		{
			PreviewMesh->SetStaticMesh(Buildable.Mesh);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Buildable Mesh is null"));
		}

		// 콜리전 비활성화
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
		// 머티리얼이 많은 물체도 있을거니까 그게 다 녹색이든 빨간색이든 바뀌어야하니까 몇개 머티리얼 있는지 보고 / GetNumMaterials()로 PreviewMesh의 머티리얼 개수 가져옴
		int32 NumMaterials = PreviewMesh->GetNumMaterials();
		for (int32 Index = 0; Index < NumMaterials; ++Index)
		{
			// 지정된 인덱스에 있는 머티리얼을 새로운 머티리얼인 MaterialToApply로 설정
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

	// BuildID가 유효한지 확인
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
	// BuildID 로그 출력
	//UE_LOG(LogTemp, Warning, TEXT("Launching Build Mode with BuildID: %d"), BuildID);

	//// BuildID가 유효한지 검사
	//if (BuildID < 0 || BuildID >= Buildables.Num())
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Invalid BuildID: %d"), BuildID);
	//	// 기본값으로 설정하거나 사용자에게 알림 제공
	//	BuildID = 0; // 예시로 기본값을 0으로 설정
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
	//	// 인벤토리의 아이템과 하우징 데이터 테이블의 RowName을 비교하여 BuildID 설정
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
	//		SpawnPreviewMesh();  // 프리뷰 메쉬 스폰
	//		BuildCycle();  // BuildCycle 시작
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
	// 		// 기본값으로 설정하거나 사용자에게 알림 제공
	// 		BuildID = 0; // 예시로 기본값을 0으로 설정
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
			// 테스트
			//SpawnedActor->Tags.Add(FName(*PlayerRef->MyPlayerController->playerName));

			// UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("UHousingComponent :: SpawnBuild :: SpawnedActor : %p"), SpawnedActor));

			if (!SpawnedActor)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to spawn actor"));
			}
			// 스폰된 액터가 HousingInterface를 구현하고 있는지 확인
			if (SpawnedActor->GetClass()->ImplementsInterface(UHousingInterface::StaticClass()))
			{
				// 인터페이스 함수 호출
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

	// 허공에 생기는 것을 막기 위해 바닥 또는 벽 충돌 여부를 확인
	if (!bHit || !HitResult.GetComponent())
	{
		return false;
	}

	// 바닥에 설치 가능한 경우 추가 확인
	if (TraceChannel == ECC_GameTraceChannel2)
	{
		if (!HitResult.GetComponent()->ComponentHasTag("Floor")) /*GetActor()->ActorHasTag("Floor"))*/
		{
			return false;
		}
	}

	// 벽에 설치 가능한 경우 추가 확인
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
			// 인터페이스 함수 호출
			BuildID = IHousingInterface::Execute_ReturnBuildID(MoveableActor);
			UE_LOG(LogTemp, Warning, TEXT("Build ID: %d"), BuildID);

			FName ItemID = Buildables[BuildID].ID;
			this->BuildID = BuildID;

			LaunchBuildMode(ItemID);


		}
		else
		{
			// 여기서 movable이 아닌 액터가 Hit된경우 처리
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