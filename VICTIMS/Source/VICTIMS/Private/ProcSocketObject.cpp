// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcSocketObject.h"

UProcSocketObject::UProcSocketObject()
	: Socket(nullptr)
{
}

bool UProcSocketObject::DequeueReceivedString(FString& OutString)
{
	return ReceivedDataQueue.Dequeue(OutString);
}

void UProcSocketObject::SendData(FString Message)
{
	if (!Socket) return;

	TArray<uint8> SendData;
	SendData.Append(reinterpret_cast<const uint8*>(TCHAR_TO_ANSI(*Message)), Message.Len());

	int32 Sent;
	Socket->Send(SendData.GetData(), SendData.Num(), Sent);
}

void UProcSocketObject::ReceiveData()
{
	if (!Socket) return;

    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]()
        {
            TArray<uint8> ReceivedData;
            uint32 Size;

            while (true)  // 무한 루프로 지속적으로 데이터를 확인합니다.
            {
                if (Socket->HasPendingData(Size))  // 소켓에 대기 중인 데이터가 있는지 확인합니다.
                {
                    ReceivedData.SetNumUninitialized(Size);
                    int32 Read = 0;
                    Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);

                    FString ReceivedString = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));
                    UE_LOG(LogTemp, Log, TEXT("UProcSocketObject::Received Data: %s"), *ReceivedString);

                    // 큐에 데이터 추가
                    ReceivedDataQueue.Enqueue(ReceivedString);
                }
                else
                {
                    // 데이터를 기다리는 짧은 대기 시간을 추가합니다.
                    FPlatformProcess::Sleep(0.01f);
                }
            }
        });
}