
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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")		// 사용방식 텍스트 가져오기
	FText GetUseActionText(); 
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")		// 사용가능 확인
	bool GetIsActorUsable();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Default")			// 상호작용 가능 액터 아웃라인	시작
	bool BeginOutlineFocus();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Default")			// 상호작용 가능 액터 아웃라인 끝
	bool EndOutlineFocus();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")		// 상호작용 액터 사용
	bool OnActorUsed(APlayerController* Controller);
};
