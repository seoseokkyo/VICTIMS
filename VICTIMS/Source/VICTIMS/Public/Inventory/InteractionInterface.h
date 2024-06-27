
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

class AVICTIMSCharacter;

UENUM()
enum class EInteractableType : uint8									// ��ȣ�ۿ� ����
{	
	Pickup					UMETA(DisplayName = "PickUp"),					// �ݱ�
	Store					UMETA(DisplayName = "Store"),					// ����
	Device					UMETA(DisplayName = "Device"),					// ����̽� (�Ƹ� �� ��ȣ�ۿ����� ���?)
	Toggle					UMETA(DisplayName = "Toggle"),					// ���
	Container				UMETA(DisplayName = "Container"),				// ������ 
	NonPlayerCharacter		UMETA(DisplayName = "NonPlayerCharacter")		// ��Ÿ ��Ȳ
};

USTRUCT()
struct FInteractableData
{
	GENERATED_USTRUCT_BODY()

	FInteractableData() :							// �ݴ� ������ �ʿ����� 
		InteractableType(EInteractableType::Pickup),
		Name(FText::GetEmpty()),
		Action(FText::GetEmpty()),
		Quantity(0),
		InteractionDuration(0.0f){};
				
	UPROPERTY(EditInstanceOnly)						// ��ȣ�ۿ� ���� ����
	EInteractableType InteractableType;
	UPROPERTY(EditInstanceOnly)						// �̸� 
	FText Name;
	UPROPERTY(EditInstanceOnly)						// ��ȣ�ۿ� �׼� 
	FText Action;
	UPROPERTY(EditInstanceOnly)						// �ݴ� ������ - ���� 
	int8 Quantity;
	UPROPERTY(EditInstanceOnly)						// ��ȣ�ۿ� �Ϸ���� �ɸ��� �ð� 
	float InteractionDuration;
};
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};
class VICTIMS_API IInteractionInterface	
{
	GENERATED_BODY()

public:												// ��ȣ�ۿ� ���� 
	virtual void BeginFocus();
	virtual void EndFocus();
	virtual void BeginInteract();
	virtual void EndInteract();
	virtual void Interact(AVICTIMSCharacter* PlayerCharacter);

	FInteractableData InteractableData;
};
