// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "CharacterBase.h"
#include "VICTIMSCharacter.generated.h"

class AVICTIMSPlayerController;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UHPWidget;
class UCollisionComponent;
struct FInputActionValue;

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

	// ToggleCombat(있는경우 사용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ia_ToggleCombat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ia_LeftClickAction;

public:
//=====================================================================================================
//  인벤토리, 아이템, 상호작용 관련 
//=====================================================================================================

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleInventory;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleProfile;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleMenu;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Interact;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleUIMode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* UserHotbar1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* UserHotbar2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* UserHotbar3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* UserHotbar4;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* UserHotbar5;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveObjectAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RemoveObjectAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SaveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CloseLayoutAction;

	/***********/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* HousingBuildAction;
	
	UPROPERTY()
	UHPWidget* hpWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UHPWidget> hpWidget_bp;

	UPROPERTY()
	class UPlayerDiedWidget* DiedWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UPlayerDiedWidget> DiedWidget_bp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")			// 상호작용 범위
	class USphereComponent* InteractionField;

	UPROPERTY()
	class AVICTIMSPlayerController* MyPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* MainWeapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Chest;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Feet;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Hands;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Legs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Head;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Interaction")
	TArray<AActor*> UsableActorsInsideRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	TArray<AActor*> WorldActorsInsideRange;
	
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing="OnRep_MainWeaponMesh", meta=(DisplayName="Weapon Mesh", Category="Inventory|Equipment"))
	USkeletalMesh* MainWeaponMesh;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing="OnRep_MainChestMesh", meta=(DisplayName="Main Chest Mesh", Category="Inventory|Equipment"))
	USkeletalMesh* ChestMesh;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing="OnRep_MainFeetMesh", meta=(DisplayName="Main Feet Mesh", Category="Inventory|Equipment"))
	USkeletalMesh* FeetMesh;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing="OnRep_MainHandsMesh", meta=(DisplayName="Main Hands Mesh", Category="Inventory|Equipment"))
	USkeletalMesh* HandsMesh;
	
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing="OnRep_MainLegsMesh", meta = (DisplayName = "Main Legs Mesh", Category = "Inventory|Equipment"))
	USkeletalMesh* LegsMesh;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing="OnRep_MainHeadMesh", meta = (DisplayName = "Main Head Mesh", Category = "Inventory|Equipment"))
	USkeletalMesh* HeadMesh;	

//=====================================================================================================
//  FUNCTION

	UFUNCTION(meta=(OverrideNativeName="OnRep_MainWeaponMesh"))
	void OnRep_MainWeaponMesh();

	UFUNCTION(meta=(OverrideNativeName="OnRep_MainChestMesh"))
	void OnRep_MainChestMesh();
	
	UFUNCTION(meta=(OverrideNativeName="OnRep_MainFeetMesh"))
	void OnRep_MainFeetMesh();

	UFUNCTION(meta=(OverrideNativeName="OnRep_MainHandsMesh"))
	void OnRep_MainHandsMesh();

	UFUNCTION(meta = (OverrideNativeName = "OnRep_MainLegsMesh"))
	void OnRep_MainLegsMesh();

	UFUNCTION(meta = (OverrideNativeName = "OnRep_MainHeadMesh"))
	void OnRep_MainHeadMesh();

	UFUNCTION(BlueprintCallable)
	void OnBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
//=====================================================================================================

	AVICTIMSCharacter();
	
protected:

//=====================================================================================================
// 상호작용 관련 
//=====================================================================================================


	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);


	void CharacterJump(const FInputActionValue& Value);
	void ToggleCombat(const FInputActionValue& Value);

	void LeftClick(const FInputActionValue& Value);
	void OnRightMouseButtonPressed(const FInputActionValue& Value);

	void PrintInfo();


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; };
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; };
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Build")
    class UHousingComponent* HousingComponent;

	UFUNCTION(BlueprintCallable, Category = "Build")
	void DestroyComponent(UActorComponent* TargetComponent);
	

	UFUNCTION(Server, Reliable)
	void Server_RemoveObject();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RemoveObject();

	UFUNCTION(Server, Reliable)
	void Server_MoveObject();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_MoveObject();

	UFUNCTION()
	void OnMoveObject();

	UFUNCTION()
	void OnRemoveObject();

	UFUNCTION(Server, Reliable)
	void Server_SpawnBuild(UHousingComponent* Comp, bool Moving, AActor* Movable, const FTransform& Transform, const TArray<FBuildablesStructs>& DB, int32 ID);


	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "weapon")
	//TSubclassOf<class ABaseWeapon> defaultWeapon;

	UFUNCTION(Server, Reliable)
	void ServerRPC_ToggleCombat();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPC_ToggleCombat(bool bCombatEnable);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	virtual void DieFunction() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Category = "House")
	class AShelter* AssignedHouse;

	void SetAssignedHouse(AShelter* NewHouse);

	UFUNCTION(Server, Reliable)
	void ServerRPC_SetAssignedHouse(AShelter* NewHouse);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPC_SetAssignedHouse(AShelter* NewHouse);

	// 	UFUNCTION(BlueprintCallable)
	// 	void GoToHouse();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_GoToHouse();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MultiCast_GoToHouse(AShelter* NewHouse);



//=====================================================================================================
// Save

public:
	UPROPERTY()			// 저장시 사용하는 개인 ID 
	FString PersonalID; 

	UFUNCTION()
	void SavePersonalID(FString ID);
	
	UPROPERTY()
	class UTestSaveGame* SavedData;

	UFUNCTION()
	void SaveDataNow();

	UFUNCTION()			// 플레이어 정보 데이터 저장
	void SavePlayerData(UTestSaveGame* Data);
	
	UFUNCTION()			// 플레이어 정보 데이터 로드
	void LoadPlayerData(UTestSaveGame* Data);

	//che
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")

	UCollisionComponent* collisionComponent;

	UFUNCTION(BlueprintImplementableEvent)
	void AddDamage(ACharacterBase* DamagedChar);

	UFUNCTION(BlueprintCallable)
	void KillWidgetOn(ACharacterBase* DiedChar);

	UFUNCTION(Server, Reliable)
	void ServerRPC_KillWidget(ACharacterBase* DiedChar);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPC_KillWidget(ACharacterBase* DiedChar);


	UPROPERTY(EditAnywhere)
	class USoundBase* KillSound;

};

