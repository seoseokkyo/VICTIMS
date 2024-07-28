#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "PartyComponent.generated.h"

class AVICTIMSCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VICTIMS_API UPartyComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPartyComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRPC_CreateNewParty(AVICTIMSCharacter* Leader, const FString& Name);
    void ServerRPC_CreateNewParty_Implementation(AVICTIMSCharacter* Leader, const FString& Name);
    bool ServerRPC_CreateNewParty_Validate(AVICTIMSCharacter* Leader, const FString& Name);

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRPC_BreakParty();
    void ServerRPC_BreakParty_Implementation();
    bool ServerRPC_BreakParty_Validate();

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRPC_LeaveParty(AVICTIMSCharacter* Member);
    void ServerRPC_LeaveParty_Implementation(AVICTIMSCharacter* Member);
    bool ServerRPC_LeaveParty_Validate(AVICTIMSCharacter* Member);

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRPC_AddNewPartyMember(AVICTIMSCharacter* NewMember);
    void ServerRPC_AddNewPartyMember_Implementation(AVICTIMSCharacter* NewMember);
    bool ServerRPC_AddNewPartyMember_Validate(AVICTIMSCharacter* NewMember);

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerRPC_preBreakParty();
    void ServerRPC_preBreakParty_Implementation();
    bool ServerRPC_preBreakParty_Validate();

    UPROPERTY(ReplicatedUsing = OnRep_PartyLeader, BlueprintReadWrite)
    AVICTIMSCharacter* PartyLeader;

    UPROPERTY(ReplicatedUsing = OnRep_PartyMembers, BlueprintReadWrite)
    TArray<AVICTIMSCharacter*> PartyMembers;

    UPROPERTY(ReplicatedUsing = OnRep_PartyName, BlueprintReadWrite)
    FString PartyName;

    UPROPERTY(ReplicatedUsing = OnRep_bOnParty, BlueprintReadWrite)
    bool bOnParty;

protected:
    UFUNCTION()
    void OnRep_PartyLeader();

    UFUNCTION()
    void OnRep_PartyMembers();

    UFUNCTION()
    void OnRep_PartyName();

    UFUNCTION()
    void OnRep_bOnParty();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
