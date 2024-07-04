// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "CharacterBase.h"
#include "InteractionInterface.h"
#include "VICTIMSCharacter.generated.h"

class AVICTIMSPlayerController;
class AMainHUD;
class UItemBase;
class IInteractionInterface;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

USTRUCT()
struct FInteractionData						// ��ȣ�ۿ� ���� �ʿ� ���
{
	GENERATED_USTRUCT_BODY()

	FInteractionData() :
		CurrentInteractable(nullptr),
		LastInteractionCheckTime(0.0f){};

	UPROPERTY()
	TObjectPtr<AActor> CurrentInteractable;
	UPROPERTY()
	float LastInteractionCheckTime;
};


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AVICTIMSCharacter : public ACharacterBase
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleMenuAction;

	// ToggleCombat(�ִ°�� ���)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ia_ToggleCombat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ia_LeftClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ia_ToggleCrouch;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)			// ��ȣ�ۿ� ���ɹ��� 
	class USphereComponent* interactableRange;	
	
	bool bInteracting;									// ��ȣ�ۿ� �������� Ȯ��
//=====================================================================================================
//  FUNCTION
//=====================================================================================================

	FORCEINLINE bool IsInteracting() const { return GetWorldTimerManager().IsTimerActive(TimerHandle_Interaction); };
	FORCEINLINE class UInventoryComponent* GetInventory() const { return PlayerInventory; };	// �κ��丮 ��������

	void UpdateInteractionWidget() const;				// ��ȣ�ۿ� ���� ������Ʈ
	void DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop);		// ������ ������

	AVICTIMSCharacter();
	
protected:
	UPROPERTY()
	AMainHUD* HUD;
	UPROPERTY()
	AVICTIMSPlayerController* MainPlayerController;
//=====================================================================================================
// ��ȣ�ۿ� ���� 
//=====================================================================================================

	UPROPERTY(VisibleAnywhere, Category = "Character | Interaction")
	TScriptInterface<IInteractionInterface> TargetInteractable;
	UPROPERTY(VisibleAnywhere, Category= "Character | Inventory")
	UInventoryComponent* PlayerInventory;
	
	UPROPERTY(EditDefaultsOnly, Category = "Character | Interaction")
	float InteractionCheckDistance = 2.0f;
	float InteractionCheckFrequency;
	FTimerHandle TimerHandle_Interaction;
	FInteractionData InteractionData;

//=====================================================================================================

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);


	void CharacterJump(const FInputActionValue& Value);
	void ToggleCombat(const FInputActionValue& Value);

	void LeftClick(const FInputActionValue& Value);

	void Crouch(const FInputActionValue& Value);

	void PrintInfo();


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void ToggleMenu();

	void FoundInteractable(AActor* NewInteractable);
	void NoInteractableFound();
	void BeginInteract();
	void EndInteract();
	void Interact();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; };
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; };

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Build")
    class UHousingComponent* HousingComponent;

	UFUNCTION(BlueprintCallable, Category = "Build")
	void DestroyComponent(UActorComponent* TargetComponent);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "weapon")
	TSubclassOf<class ABaseWeapon> defaultWeapon;

	UFUNCTION(Server, Reliable)
	void ServerRPC_ToggleCombat();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPC_ToggleCombat();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	virtual void DieFunction() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
		// ��ȣ�ۿ� ���ɹ��� �Ǻ� 
	UFUNCTION()
	void OnBeginOverlapInteractableRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlapInteractableRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};

