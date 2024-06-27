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

UHousingComponent::UHousingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	IsBuildModeOn = false;
	CanBuild = false;
	BuildID = 0;
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
		TArray<FName> RowNames = DB_Housing->GetRowNames();

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

	// 	BuildID = 0;
	// 
	// 	// Buildables 배열 크기 확인 로그
	// 	UE_LOG(LogTemp, Warning, TEXT("Buildables array size: %d"), Buildables.Num());

		//     if (DB_Buildables)
		//     {
		//         // 데이터 테이블의 행 이름 가져오기
		//         TArray<FName> RowNames;
		//         UDataTableFunctionLibrary::GetDataTableRowNames(DB_Buildables, RowNames);
		// 
		//         // 각 행 이름에 대해 데이터 가져오기?
		//         for (const FName& RowName : RowNames)
		//         {
		//             // FBuildableRow* Row = DB_Buildables->FindRow<FBuildableRow>(RowName, TEXT(""));
		//             if (Row)
		//             {
		//             }
		//             else
		//             {
		//                 GLog->Log(FString::Printf(TEXT("Row not found: %s"), *RowName.ToString()));
		//             }
		//         }
		//     }
			// UDataTableFunctionLibrary::GetDataTableRowNames
}

void UHousingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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

	// UStaticMeshComponent* NewStaticMeshComponent = Cast<UStaticMeshComponent>(PlayerRef->AddComponent(TEXT("PreviewMesh"), false, BuildTransform, UStaticMeshComponent::StaticClass()));
	// UStaticMeshComponent* NewStaticMeshComponent = NewObject<UStaticMeshComponent>(PlayerRef);
	PreviewMesh = NewObject<UStaticMeshComponent>(PlayerRef);

	if (PreviewMesh)
	{
		PreviewMesh->AttachToComponent(PlayerRef->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		PreviewMesh->SetRelativeTransform(BuildTransform);
		PreviewMesh->RegisterComponent();
		//PreviewMesh = NewStaticMeshComponent;

		//         if (PreviewMeshAsset)
		//         {
		//             PreviewMesh->SetStaticMesh(PreviewMeshAsset);
		//         }
		//         else
		//         {
		//             UE_LOG(LogTemp, Warning, TEXT("PreviewMeshAsset is null"));
		//         }
		// 
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
		//PreviewMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);






		GiveBuildColor();






	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create Static Mesh Component"));
	}
}

void UHousingComponent::GiveBuildColor(/*bool IsGreen*/)
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
	//UMaterialInterface* MaterialToApply = IsGreen ? GreenMaterial : RedMaterial;






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


	FVector StartLocation = Camera->GetComponentLocation() + Camera->GetForwardVector() * 350.f;
	// FVector ForwardVector = Camera->GetForwardVector();
	FVector EndLocation = Camera->GetComponentLocation() + Camera->GetForwardVector() * 1000.f;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	ECollisionChannel temp = UEngineTypes::ConvertToCollisionChannel(Buildable.TraceChannel);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, temp, Params);

	//if (bHit)
	//{
	//    BuildTransform.SetLocation(HitResult.ImpactPoint);
	//    // BuildTransform.SetRotation(FQuat(HitResult.ImpactNormal.Rotation()));
	//    if (IsValid(PreviewMesh))
	//    {
	//        GiveBuildColor(true);
	//        BuildDelay();
	//    }
	//    else
	//    {
	//        SpawnPreviewMesh();
	//        BuildDelay();
	//    }
	//}
	//else
	//{
	//    BuildTransform.SetLocation(EndLocation);
	//    // BuildTransform.SetRotation(FQuat(ForwardVector.Rotation()));
	//    if (IsValid(PreviewMesh))
	//    {
	//        GiveBuildColor(false);
	//        BuildDelay();
	//    }
	//    else
	//    {
	//        SpawnPreviewMesh();
	//        BuildDelay();
	//    }
	//}
	if (bHit)
	{
		FVector ImpactPoint = HitResult.ImpactPoint;

		BuildTransform.SetLocation(ImpactPoint);
	}
	else
	{
		BuildTransform.SetLocation(EndLocation);
	}

	//BuildTransform.SetRotation(FQuat(ForwardVector.Rotation()));

	if (IsValid(PreviewMesh))
	{
		GiveBuildColor(/*true*/);
	}
	else
	{
		SpawnPreviewMesh();
		GiveBuildColor(/*false*/);
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
	if (IsValid(PreviewMesh))
	{
		PlayerRef->DestroyComponent(PreviewMesh);
		PreviewMesh = nullptr;
	}
}

void UHousingComponent::LaunchBuildMode()
{
	// BuildID 로그 출력
	UE_LOG(LogTemp, Warning, TEXT("Launching Build Mode with BuildID: %d"), BuildID);

	// BuildID가 유효한지 검사
	if (BuildID < 0 || BuildID >= Buildables.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid BuildID: %d"), BuildID);
		// 기본값으로 설정하거나 사용자에게 알림 제공
		BuildID = 0; // 예시로 기본값을 0으로 설정
		return;
	}

	if (IsBuildModeOn)
	{
		StopBuildMode();
	}
	else
	{
		IsBuildModeOn = true;
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

void UHousingComponent::SpawnBuild()
{
	if (BuildID < 0 || BuildID >= Buildables.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid BuildID: %d"), BuildID);
		// 기본값으로 설정하거나 사용자에게 알림 제공
		BuildID = 0; // 예시로 기본값을 0으로 설정
		return;
	}

	const FBuildablesStructs& Buildable = Buildables[BuildID];

	if (Buildable.Actor)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(Buildable.Actor, BuildTransform, SpawnParams);
		if (!SpawnedActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn actor"));
		}
		// 스폰된 액터가 HousingInterface를 구현하고 있는지 확인
		if (SpawnedActor->GetClass()->ImplementsInterface(UHousingInterface::StaticClass()))
		{
			// 인터페이스 함수 호출
			//IHousingInterface::SetMesh_Implementation(SpawnedActor, Buildable.Mesh);
			IHousingInterface::Execute_SetMesh(SpawnedActor, Buildable.Mesh);

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
	CollisionParams.AddIgnoredActor(PlayerRef);

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
	if (!bHit || !HitResult.GetActor())
	{
		return false;
	}

	// 바닥에 설치 가능한 경우 추가 확인
	if (TraceChannel == ECC_GameTraceChannel2)
	{
		if (!HitResult.GetActor()->ActorHasTag("Floor"))
		{
			return false;
		}
	}

	// 벽에 설치 가능한 경우 추가 확인
	if (TraceChannel == ECC_GameTraceChannel3)
	{
		if (!HitResult.GetActor()->ActorHasTag("Wall"))
		{
			return false;
		}
	}

	return true;
}

