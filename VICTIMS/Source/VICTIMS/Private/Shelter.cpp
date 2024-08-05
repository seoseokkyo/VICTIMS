// Fill out your copyright notice in the Description page of Project Settings.


#include "Shelter.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "HousingInterface.h"
#include "HousingComponent.h"
#include "BuildSaveData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/OverlapResult.h"

AShelter::AShelter()
{
	PrimaryActorTick.bCanEverTick = false;

	// 초기화
	//HouseNumber = -1;

	// origin comp pos(X = 0.000000, Y = 230.000000, Z = 3001.000000);    
	OriginPosComponent = CreateDefaultSubobject<USceneComponent>(TEXT("OriginPosComponent"));

	if (RootComponent)
	{
		OriginPosComponent->SetupAttachment(RootComponent);
	}
	else
	{
		SetRootComponent(OriginPosComponent);
	}

	OriginPosComponent->SetRelativeLocation(FVector(0.0f, 230.0f, 3001.0f));
	OriginPosComponent->ComponentTags.Add(TEXT("OriginPos"));

	// searchregion pos (X=208.000000,Y=0.000000,Z=316.000000)
	// searchregion extent(X=1400.000000,Y=1000.000000,Z=400.000000)
	SearchRegion = CreateDefaultSubobject<UBoxComponent>(TEXT("SearchRegionComp"));
	SearchRegion->SetupAttachment(OriginPosComponent);

	SearchRegion->SetRelativeLocation(FVector(208.0f, 0.0f, 316.0f));
	SearchRegion->SetBoxExtent(FVector(1400.0f, 1000.0f, 400.0f));
}

void AShelter::BeginPlay()
{
	Super::BeginPlay();

	if (DB_Housing)
	{
		RowNames = DB_Housing->GetRowNames();

		for (const FName& RowName : RowNames)
		{
			FBuildablesStructs* RowData = DB_Housing->FindRow<FBuildablesStructs>(RowName, TEXT(""));

			if (RowData)
			{
				Buildables.Add(*RowData);
			}
		}
	}

	if (DB_Item)
	{
		ItemRowNames = DB_Item->GetRowNames();
	}
}

void AShelter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShelter::SetPlayerName(FString ID)
{
	ServerRPC_SetPlayerName(ID);
}

void AShelter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShelter, OwnerPlayerName);

}

TArray<FBuildSaveDataStruct> AShelter::SearchBuildItem()
{
	TArray<FBuildSaveDataStruct> searched;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	//QueryParams.bDebugQuery = true;

	bool bOverlap = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		SearchRegion->GetComponentLocation(),
		FQuat::Identity,
		ECC_Visibility, // 검색할 Collision 채널
		FCollisionShape::MakeBox(SearchRegion->GetScaledBoxExtent()), // 검색할 범위 (구체)
		QueryParams
	);

	if (bOverlap)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* OverlappedActor = Result.GetActor();
			if (OverlappedActor && OverlappedActor->ActorHasTag(TEXT("PlayerBuild")))
			{
				if (OverlappedActor->GetClass()->ImplementsInterface(UHousingInterface::StaticClass()))
				{
					FBuildSaveDataStruct temp;

					int32 RemovedBuildID = IHousingInterface::Execute_ReturnBuildID(OverlappedActor);
					FName RemovedItemID = Buildables[RemovedBuildID].ID;

					auto actorLoc = OverlappedActor->GetActorLocation();
					auto actorRot = OverlappedActor->GetActorRotation();

					FVector calcedPos = OriginPos - actorLoc;

					temp.ID_Name = RemovedItemID;
					temp.ID_Num = RemovedBuildID;
					temp.location = calcedPos;
					temp.rotation = actorRot;

					//temp.transform = actorTransform;
					
					//OverlappedActor->Destroy();

					searched.Add(temp);
				}
			}
		}
	}

	return searched;
}

void AShelter::DestroyBuildItem()
{
	TArray<FBuildSaveDataStruct> searched;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	//QueryParams.bDebugQuery = true;

	bool bOverlap = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		SearchRegion->GetComponentLocation(),
		FQuat::Identity,
		ECC_Visibility, // 검색할 Collision 채널
		FCollisionShape::MakeBox(SearchRegion->GetScaledBoxExtent()), // 검색할 범위 (구체)
		QueryParams
	);

	if (bOverlap)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* OverlappedActor = Result.GetActor();
			if (OverlappedActor && OverlappedActor->ActorHasTag(TEXT("PlayerBuild")))
			{
				if (OverlappedActor->GetClass()->ImplementsInterface(UHousingInterface::StaticClass()))
				{
					OverlappedActor->Destroy();
				}
			}
		}
	}
}

void AShelter::ServerRPC_SetPlayerName_Implementation(const FString& ID)
{
	OwnerPlayerName = ID;

	NetMulticastRPC_SetPlayerName(ID);
}

void AShelter::NetMulticastRPC_SetPlayerName_Implementation(const FString& ID)
{
	OwnerPlayerName = ID;
}

void AShelter::ServerRPC_SetOriginPos_Implementation(FVector houseLocation)
{
	OriginPos = houseLocation;
}
