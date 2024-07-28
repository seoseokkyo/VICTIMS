#include "PartyComponent.h"
#include "Net/UnrealNetwork.h"
#include "VictimsCharacter.h"

UPartyComponent::UPartyComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
}

void UPartyComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UPartyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPartyComponent::ServerRPC_CreateNewParty_Implementation(AVICTIMSCharacter* Leader, const FString& Name)
{
    PartyLeader = Leader;
    PartyName = Name;
    bOnParty = true;
    PartyMembers.Add(Leader);
}

bool UPartyComponent::ServerRPC_CreateNewParty_Validate(AVICTIMSCharacter* Leader, const FString& Name)
{
    return true;
}


void UPartyComponent::ServerRPC_preBreakParty_Implementation()
{   

	
            for (AActor* Member : PartyMembers)
            {
                if (Member)
                {
                    ServerRPC_BreakParty(); // 각 멤버에서 이 함수를 호출
                }
            }
            ServerRPC_BreakParty_Implementation();
 }
    



bool UPartyComponent::ServerRPC_preBreakParty_Validate()
{
    return true;
}

void UPartyComponent::ServerRPC_BreakParty_Implementation()
{
    PartyLeader = nullptr;
    PartyName = "";
    bOnParty = false;
    PartyMembers.Empty();
}




bool UPartyComponent::ServerRPC_BreakParty_Validate()
{
    return true;
}

void UPartyComponent::ServerRPC_LeaveParty_Implementation(AVICTIMSCharacter* Member)
{
    PartyMembers.Remove(Member);
    if (PartyMembers.Num() == 0)
    {
        ServerRPC_BreakParty();
    }
}

bool UPartyComponent::ServerRPC_LeaveParty_Validate(AVICTIMSCharacter* Member)
{
    return true;
}

void UPartyComponent::ServerRPC_AddNewPartyMember_Implementation(AVICTIMSCharacter* NewMember)
{
    if (PartyMembers.Num() < 5)
    {
        PartyMembers.Add(NewMember);
    }
}

bool UPartyComponent::ServerRPC_AddNewPartyMember_Validate(AVICTIMSCharacter* NewMember)
{
    return true;
}

void UPartyComponent::OnRep_PartyLeader()
{
    // Party leader updated logic
}

void UPartyComponent::OnRep_PartyMembers()
{
    // Party members updated logic
}

void UPartyComponent::OnRep_PartyName()
{
    // Party name updated logic
}

void UPartyComponent::OnRep_bOnParty()
{
    // Party state updated logic
}

void UPartyComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UPartyComponent, PartyLeader);
    DOREPLIFETIME(UPartyComponent, PartyMembers);
    DOREPLIFETIME(UPartyComponent, PartyName);
    DOREPLIFETIME(UPartyComponent, bOnParty);
}
