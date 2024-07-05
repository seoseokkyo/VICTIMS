
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UsableActorInterface.generated.h"

UINTERFACE()
class UUsableActorInterface : public UInterface
{
	GENERATED_BODY()
};


class VICTIMS_API IUsableActorInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")		// ����� �ؽ�Ʈ ��������
	FText GetUseActionText(); 
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")		// ��밡�� Ȯ��
	bool GetIsActorUsable();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Default")			// ��ȣ�ۿ� ���� ���� �ƿ�����	����
	bool BeginOutlineFocus();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Default")			// ��ȣ�ۿ� ���� ���� �ƿ����� ��
	bool EndOutlineFocus();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")		// ��ȣ�ۿ� ���� ���
	bool OnActorUsed(APlayerController* Controller);
};
