// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerProcSocket.h"

UServerProcSocket::UServerProcSocket()
{
}

void UServerProcSocket::InitializeSocket()
{
    Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("ServerProcSocket"), false);

    TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    Addr->SetAnyAddress();
    Addr->SetPort(8001);

    bool bBind = Socket->Bind(*Addr);
    if (bBind)
    {
        Socket->Listen(8); // Queue size of 8
        UE_LOG(LogTemp, Log, TEXT("Server started on port 8001"));
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UServerProcSocket::HandleIncomingConnections, 1.0f, true);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to bind server socket"));
    }
}

void UServerProcSocket::HandleIncomingConnections()
{
    if (!Socket) return;

    bool PendingConnection = false;

    if (Socket->HasPendingConnection(PendingConnection) && PendingConnection)
    {
        TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
        FSocket* ClientSocket = Socket->Accept(*RemoteAddress, TEXT("AcceptedConnection"));
        if (ClientSocket)
        {
            UE_LOG(LogTemp, Log, TEXT("Accepted connection from %s"), *RemoteAddress->ToString(true));

            bConnection = true;

            ClientSockets.Add(ClientSocket);
            HandleClient(ClientSocket);
        }
    }
}

void UServerProcSocket::HandleClient(FSocket* ClientSocket)
{
    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [ClientSocket, this]()
        {
            TArray<uint8> ReceivedData;
            uint32 Size;

            while (true)  // 무한 루프로 지속적으로 데이터를 확인합니다.
            {
                if (ClientSocket->HasPendingData(Size))  // 소켓에 대기 중인 데이터가 있는지 확인합니다.
                {
                    ReceivedData.SetNumUninitialized(Size);
                    int32 Read = 0;
                    ClientSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);

                    FString ReceivedString = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));
                    UE_LOG(LogTemp, Log, TEXT("Received Data: %s"), *ReceivedString);

                    // 큐에 데이터 추가
                    ReceivedDataQueue.Enqueue(ReceivedString);

                    // 클라이언트로 응답 보내기
                    FString ResponseString = TEXT("Hello from server!");
                    TArray<uint8> ResponseData;
                    ResponseData.Append(reinterpret_cast<const uint8*>(TCHAR_TO_ANSI(*ResponseString)), ResponseString.Len());
                    int32 Sent;
                    ClientSocket->Send(ResponseData.GetData(), ResponseData.Num(), Sent);
                }
                else
                {
                    // 데이터를 기다리는 짧은 대기 시간을 추가합니다.
                    FPlatformProcess::Sleep(0.01f);
                }
            }
        });
}
