// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProcSocketObject.h"
#include "InstantProcSocket.generated.h"

/**
 *
 */
UCLASS()
class VICTIMS_API UInstantProcSocket : public UProcSocketObject
{
	GENERATED_BODY()

public:
	UInstantProcSocket();

	UPROPERTY()
	int32 InstantProcNumber = 0;

	UPROPERTY()
	int32 InstantProcPortNum = 0;

	void SetInstantProcNumber(int32 number) { InstantProcNumber = number; };
	int32 GetInstantProcNumber() { return InstantProcNumber; };

	void SetInstantProcPortNum(int32 number) { InstantProcPortNum = number; };
	int32 GetInstantProcPortNum() { return InstantProcPortNum; };

	virtual void InitializeSocket() override;

	void SendDataToServer();
};
