// Fill out your copyright notice in the Description page of Project Settings.


#include "InstantProcSocket.h"

UInstantProcSocket::UInstantProcSocket()
{
}

void UInstantProcSocket::InitializeSocket()
{
    Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("InstantProcSocket"), false);

    FString Address = TEXT("192.168.0.147");
    int32 Port = 8001;

    UE_LOG(LogTemp, Log, TEXT("Connected to server at %d"), Port);

    TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    bool bIsValid;
    RemoteAddress->SetIp(*Address, bIsValid);
    RemoteAddress->SetPort(Port);

    if (bIsValid)
    {
        bool bConnected = Socket->Connect(*RemoteAddress);
        if (bConnected)
        {
            UE_LOG(LogTemp, Log, TEXT("Connected to server at %s"), *RemoteAddress->ToString(true));
            SendDataToServer();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to connect to server"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid address"));
    }
}

void UInstantProcSocket::SendDataToServer()
{
    FString Message = FString::Printf(TEXT("InstantProcSocket_Completed_Port_%d"), InstantProcPortNum);
    SendData(Message);

    // 서버로부터 응답 받기
    ReceiveData();
}

