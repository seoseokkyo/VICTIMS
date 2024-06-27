#pragma once

#include "CoreMinimal.h"
//#include "BlajackMainWidget.h"
//#include "GameFramework/Pawn.h"
#include "BlackjackPlyaer.generated.h"

class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UPickAceValueUserWidget;
class UBlajackMainWidget;

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

	UFUNCTION(BlueprintCallable)
	void GetCard(); 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ABlackjackCard*> PlayerCardSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ABlackjackTable* table;

	UPROPERTY()
	class USceneComponent* cardPosition;

	UFUNCTION(BlueprintCallable)
	void CalcScore(FString AddValue);

	UFUNCTION(BlueprintCallable)
	void CheckCard(ABlackjackCard* card);

	UFUNCTION(BlueprintCallable)
	void ChooseAceValue(ABlackjackCard* AceCard);

	UFUNCTION(BlueprintCallable)
	void SetAceTo1(ABlackjackCard* card);

	UFUNCTION(BlueprintCallable)
	void SetAceTo11(ABlackjackCard* card);

	UFUNCTION(BlueprintCallable)
	void Stand();


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 scoreSum=0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DealerScore=0;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UPickAceValueUserWidget* AceWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> AceWidget_BP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBlajackMainWidget* MainWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> MainWidget_BP;


	class AVICTIMSCharacter* originPlayer;
	
};
