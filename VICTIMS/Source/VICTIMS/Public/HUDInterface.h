
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HUDInterface.generated.h"

UINTERFACE(MinimalAPI)
class UHUDInterface : public UInterface
{
	GENERATED_BODY()
};


class VICTIMS_API IHUDInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UseInventoryItem(const TObjectPtr<UItemBase> item);
};
