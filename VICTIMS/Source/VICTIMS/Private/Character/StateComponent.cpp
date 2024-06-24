// Fill out your copyright notice in the Description page of Project Settings.


#include "StateComponent.h"
#include "CharacterBase.h"
#include "VictimsGameInstance.h"
#include <Net/UnrealNetwork.h>
#include <Kismet/KismetSystemLibrary.h>

// Sets default values for this component's properties
UStateComponent::UStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...


}


// Called when the game starts
void UStateComponent::BeginPlay()
{
	Super::BeginPlay();

	character = GetOwner<ACharacterBase>();

	UE_LOG(LogTemp, Warning, TEXT("StateComp GetIsReplicated() : %s"), GetIsReplicated() ? TEXT("TRUE") : TEXT("FALSE"));
	if (GetIsReplicated() != true)
	{

		SetIsReplicated(true);
	}
	// ...
}


// Called every frame
void UStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UStateComponent::InitStat()
{
	if (GetOwner() == nullptr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("GetOwner Is Null : %s, %d"), __FILE__, __LINE__);
		return;
	}

	auto gi = GetOwner()->GetGameInstance<UVictimsGameInstance>();
	if (gi != nullptr)
	{
		stat = gi->GetCharacterDataTable(character->GetName());

		currentHP = stat.MaxHP;
		currentSP = stat.MaxSP;
		currentStrength = stat.Strength;
		currentAgility = stat.Agility;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GetGameInstance Is Null"));
	}
}

float UStateComponent::GetStatePoint(EStateType stateType)
{
	switch (stateType)
	{
	case HP:
		return currentHP;
	case SP:
		return currentSP;
	default:
		//UE_LOG(LogTemp, Warning, TEXT("Type Error, %s, %d"), __FILE__, __LINE__);
		break;
	}

	return 0.0f;
}

float UStateComponent::AddStatePoint(EStateType stateType, float value)
{
	float temp = 0.0f;

	switch (stateType)
	{
	case HP:
		temp = currentHP + value;
		currentHP = temp;

		if (currentHP <= 0)
		{
			if (dieDelegate.IsBound())
			{
				dieDelegate.Execute();
			}
		}

		break;
	case SP:
		temp = currentSP + value;
		currentSP = temp;
		break;
	default:
		//UE_LOG(LogTemp, Warning, TEXT("Type Error, %s, %d"), __FILE__, __LINE__);
		break;
	}

	return temp;
}

void UStateComponent::ServerRPC_SetStatePoint_Implementation(EStateType stateType, float value)
{
	switch (stateType)
	{
	case HP:
		currentHP = value;
		break;
	case SP:
		currentSP = value;
		break;
	default:
		//UE_LOG(LogTemp, Warning, TEXT("Type Error, %s, %d"), __FILE__, __LINE__);
		break;
	}
}

void UStateComponent::NetMulticastRPC_SetStatePoint_Implementation(EStateType stateType, float value)
{
	switch (stateType)
	{
	case HP:
		currentHP = value;
		break;
	case SP:
		currentSP = value;
		break;
	default:
		//UE_LOG(LogTemp, Warning, TEXT("Type Error, %s, %d"), __FILE__, __LINE__);
		break;
	}
}

void UStateComponent::UpdateStat()
{
	// 이후 장비에 있는 스탯을 여기에 추가해줘야 함
	MaxHP = stat.MaxHP;
	MaxSP = stat.MaxSP;
}

void UStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStateComponent, currentHP);
	DOREPLIFETIME(UStateComponent, MaxHP);
	DOREPLIFETIME(UStateComponent, currentSP);
	DOREPLIFETIME(UStateComponent, MaxSP);
}

