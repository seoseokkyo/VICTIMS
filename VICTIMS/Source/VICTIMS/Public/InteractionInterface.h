
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

class AVICTIMSCharacter;

UENUM()
enum class EInteractableType : uint8									// 상호작용 종류
{	
	Pickup					UMETA(DisplayName = "PickUp"),					// 줍기
	Store					UMETA(DisplayName = "Store"),					// 상점
	Device					UMETA(DisplayName = "Device"),					// 디바이스 (아마 맵 상호작용으로 사용?)
	Toggle					UMETA(DisplayName = "Toggle"),					// 토글
	Container				UMETA(DisplayName = "Container"),				// 보관함 
	NonPlayerCharacter		UMETA(DisplayName = "NonPlayerCharacter")		// 기타 상황
};

USTRUCT()
struct FInteractableData
{
	GENERATED_USTRUCT_BODY()

	FInteractableData() :							// 줍는 아이템 필요정보 
		InteractableType(EInteractableType::Pickup),
		Name(FText::GetEmpty()),
		Action(FText::GetEmpty()),
		Quantity(0),
		InteractionDuration(0.0f){};
				
	UPROPERTY(EditInstanceOnly)						// 상호작용 정보 모음
	EInteractableType InteractableType;
	UPROPERTY(EditInstanceOnly)						// 이름 
	FText Name;
	UPROPERTY(EditInstanceOnly)						// 상호작용 액션 
	FText Action;
	UPROPERTY(EditInstanceOnly)						// 줍는 아이템 - 개수 
	int8 Quantity;
	UPROPERTY(EditInstanceOnly)						// 상호작용 완료까지 걸리는 시간 
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

public:												// 상호작용 과정 
	virtual void BeginFocus();
	virtual void EndFocus();
	virtual void BeginInteract();
	virtual void EndInteract();
	virtual void Interact(AVICTIMSCharacter* PlayerCharacter);

	FInteractableData InteractableData;
};
