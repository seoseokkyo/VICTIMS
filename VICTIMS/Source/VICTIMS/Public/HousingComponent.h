// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "HousingComponent.generated.h"

 UENUM()
enum class EBuildableEnum : uint8
{
	Foundation,
	Floor,
	Wall
};

USTRUCT(/*Atomic, */BlueprintType)
struct FBuildablesStructs : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ETraceTypeQuery> TraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Actor;
};

 UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VICTIMS_API UHousingComponent : public UActorComponent
{
 	GENERATED_BODY()
 
 public:	
 	// Sets default values for this component's properties
 	UHousingComponent();
 
 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
    bool IsBuildModeOn;
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
    bool CanBuild;
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
    int32 BuildID;
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
    FTransform BuildTransform;
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class AVICTIMSCharacter* PlayerRef;
 
 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
 	class UCameraComponent* Camera;
 

 	UFUNCTION()
 	void SpawnPreviewMesh();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
    class UStaticMeshComponent* PreviewMesh;


    UFUNCTION(BlueprintCallable, Category = "Build")
    void GiveBuildColor(/*bool IsGreen*/);

    // 에디터 디테일창에서 색 지정할 수 있도록
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* GreenMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* RedMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    UStaticMesh* PreviewMeshAsset;

    UPROPERTY()
    UStaticMeshComponent* NewStaticMeshComponent;


    UFUNCTION(BlueprintCallable)
    void BuildCycle();

    UFUNCTION(BlueprintCallable)
    void StartBuildCycle();

    UFUNCTION(BlueprintCallable)
    void StopBuildCycle();

    // rerun all over again and again~
    // UFUNCTION(BlueprintCallable, Category = "Build")
    void BuildDelay();

    UFUNCTION(BlueprintCallable/*, Category = "Build"*/)
    void StopBuildMode();

    UFUNCTION(BlueprintCallable/*, Category = "Build"*/)
    void LaunchBuildMode();


 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
    UDataTable* DB_Housing;

 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
    //FBuildablesStructs S_Buildables;
    TArray<FBuildablesStructs> Buildables;


    UFUNCTION(BlueprintCallable)
    void ChangeMesh();

    UFUNCTION(BlueprintCallable)
    void SpawnBuild();

//     UFUNCTION(BlueprintCallable, Category = "Build")
//     void LoadDataFromDataTable();


    
    //UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool CanPlacePreviewMesh();





 
 protected:
 	// Called when the game starts
 	virtual void BeginPlay() override;
    FTimerHandle BuildCycleTimerHandle;
 public:	
 	// Called every frame
 	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
 
 		
};
