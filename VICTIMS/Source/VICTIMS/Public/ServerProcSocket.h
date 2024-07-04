// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProcSocketObject.h"
#include "ServerProcSocket.generated.h"

/**
 * 
 */
UCLASS()
class VICTIMS_API UServerProcSocket : public UProcSocketObject
{
	GENERATED_BODY()
	

public:
	UServerProcSocket();

	virtual void InitializeSocket() override;

	bool bConnection = false;

private:
	void HandleIncomingConnections();
	void HandleClient(FSocket* ClientSocket);
	
	
};
