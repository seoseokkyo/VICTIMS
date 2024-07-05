#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FWidgetsLayoutBP.generated.h"

USTRUCT()
struct VICTIMS_API FWidgetsLayoutBP : public FTableRowBase
{
	GENERATED_BODY()

	FWidgetsLayoutBP();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UClass* Widget;
};

