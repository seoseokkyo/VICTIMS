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
struct FInteractionData						// 상호작용 관련 필요 요소
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

	// ToggleCombat(있는경우 사용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ia_ToggleCombat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ia_LeftClickAction;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)			// 상호작용 가능범위 
	class USphereComponent* interactableRange;	
	
	bool bInteracting;									// 상호작용 가능한지 확인

//========================================================================================================
// 장비 관련
//========================================================================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* MainWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Head;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Top;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Bottom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Feet;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = "OnRep_MainWeaponMesh", meta = (DisplayName = "Weapon Mesh", Category = "Inventory|Equipment"))
	USkeletalMesh* MainWeaponMesh;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = "OnRep_MainHeadMesh", meta = (DisplayName = "Main Head Mesh", Category = "Inventory|Equipment"))
	USkeletalMesh* HeadMesh;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = "OnRep_MainTopMesh", meta = (DisplayName = "Main Top Mesh", Category = "Inventory|Equipment"))
	USkeletalMesh* TopMesh;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = "OnRep_MainBottomMesh", meta = (DisplayName = "Main Bototm Mesh", Category = "Inventory|Equipment"))
	USkeletalMesh* BottomMesh;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = "OnRep_MainFeetMesh", meta = (DisplayName = "Main Feet Mesh", Category = "Inventory|Equipment"))
	USkeletalMesh* FeetMesh;


	UFUNCTION(meta = (OverrideNativeName = "OnRep_MainWeaponMesh"))
	void OnRep_MainWeaponMesh();

	UFUNCTION(meta = (OverrideNativeName = "OnRep_MainHeadMesh"))
	void OnRep_MainHeadMesh();

	UFUNCTION(meta = (OverrideNativeName = "OnRep_MainTopMesh"))
	void OnRep_MainTopMesh();

	UFUNCTION(meta = (OverrideNativeName = "OnRep_MainBottomMesh"))
	void OnRep_MainBottomMesh();

	UFUNCTION(meta = (OverrideNativeName = "OnRep_MainFeetMesh"))
	void OnRep_MainFeetMesh();


//=====================================================================================================
//  FUNCTION
//=====================================================================================================

	FORCEINLINE bool IsInteracting() const { return GetWorldTimerManager().IsTimerActive(TimerHandle_Interaction); };
	FORCEINLINE class UInventoryComponent* GetInventory() const { return PlayerInventory; };	// 인벤토리 가져오기

	void UpdateInteractionWidget() const;				// 상호작용 위젯 업데이트
	void DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop);		// 아이템 버리기

	AVICTIMSCharacter();
	
protected:
	UPROPERTY()
	AMainHUD* HUD;
	UPROPERTY()
	AVICTIMSPlayerController* MainPlayerController;
//=====================================================================================================
// 상호작용 관련 
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

	void PrintInfo();


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

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
	
		// 상호작용 가능범위 판별 
	UFUNCTION()
	void OnBeginOverlapInteractableRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlapInteractableRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};

