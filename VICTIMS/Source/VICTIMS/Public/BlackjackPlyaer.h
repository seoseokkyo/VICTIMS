// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BlackjackPlyaer.generated.h"

class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class VICTIMS_API ABlackjackPlyaer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABlackjackPlyaer();
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftClickAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void LeftClickFunction(const FInputActionValue& Value);

	void GetCard();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ABlackjackCard*> PlayerCardSet;

	UPROPERTY()
	class ABlackjackTable* table;

	UPROPERTY()
	class USceneComponent* cardPosition;

	UFUNCTION(BlueprintCallable)
	void CalcScore();

	UPROPERTY()
	int32 scoreSum=0;
};
