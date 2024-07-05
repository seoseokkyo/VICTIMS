// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "ProcSocketObject.generated.h"

/**
 * 
 */
UCLASS()
class VICTIMS_API UProcSocketObject : public UObject
{
	GENERATED_BODY()
	
public:
    UProcSocketObject();

    virtual void InitializeSocket() PURE_VIRTUAL(UMySocketObject::InitializeSocket, );

protected:
    FSocket* Socket;
    FTimerHandle TimerHandle;

    TQueue<FString> ReceivedDataQueue;
    bool DequeueReceivedString(FString& OutString);

    void SendData(FString Message);
    void ReceiveData();
	
    UPROPERTY()
	bool bConnection = false;
};
