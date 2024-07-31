#include "InstantProcSocket.h"
#include "VictimsGameInstance.h"

UInstantProcSocket::UInstantProcSocket()
{
}

void UInstantProcSocket::InitializeSocket()
{
    Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("InstantProcSocket"), false);

    auto gameInst = GetWorld()->GetGameInstanceChecked<UVictimsGameInstance>();

    //FString Address = TEXT("192.168.0.147");
    FString Address = gameInst->mainAddress;
    int32 Port = 8001;

//     UE_LOG(LogTemp, Log, TEXT("Connected to server at %d"), Port);

    TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    bool bIsValid;
    RemoteAddress->SetIp(*Address, bIsValid);
    RemoteAddress->SetPort(Port);

    if (bIsValid)
    {
        bConnection = Socket->Connect(*RemoteAddress);
        if (bConnection)
        {
//             UE_LOG(LogTemp, Log, TEXT("Connected to server at %s"), *RemoteAddress->ToString(true));
            SendDataToServer();
        }
        else
        {
//             UE_LOG(LogTemp, Error, TEXT("Failed to connect to server"));
        }
    }
    else
    {
//         UE_LOG(LogTemp, Error, TEXT("Invalid address"));
    }
}

void UInstantProcSocket::SendDataToServer()
{
    FString Message = FString::Printf(TEXT("InstantProcSocket_Completed_Port_%d"), InstantProcPortNum);
    SendData(Message);

    // 서버로부터 응답 받기
    ReceiveData();
}

