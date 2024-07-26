
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UsableActorInterface.h"
#include "Internationalization/StringTableRegistry.h"
#include "UsableActor.generated.h"

UCLASS()
class VICTIMS_API AUsableActor : public AActor, public IUsableActorInterface
{
	GENERATED_BODY()
	
public:	
	AUsableActor();

	/* Interface */
	virtual FText GetUseActionText_Implementation() override;
	virtual bool GetIsActorUsable_Implementation() override; 
	virtual bool BeginOutlineFocus_Implementation() override;
	virtual bool EndOutlineFocus_Implementation() override;
	virtual bool OnActorUsed_Implementation(APlayerController* Controller) override;


		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Scene;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FText Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FText Action; 

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool IsUsable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = "OnRep_WasUsed")
	bool WasUsed;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, meta = (Category, OverrideNativeName = "OnWasUsed"))
	virtual bool OnWasUsed();

	UFUNCTION(BlueprintCallable, meta = (Category, OverrideNativeName = "OnRep_WasUsed"))
	virtual void OnRep_WasUsed();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUserWidget* InteractUserWidget;

	UFUNCTION()
	void SetInteractText(FText Text);
	UFUNCTION()
	void SetScreenPosition(FVector2D ScreenPosition);
};