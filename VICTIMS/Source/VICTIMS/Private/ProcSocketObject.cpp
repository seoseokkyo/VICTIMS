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

            while (true)  // ���� ������ ���������� �����͸� Ȯ���մϴ�.
            {
                if (Socket->HasPendingData(Size))  // ���Ͽ� ��� ���� �����Ͱ� �ִ��� Ȯ���մϴ�.
                {
                    ReceivedData.SetNumUninitialized(Size);
                    int32 Read = 0;
                    Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);

                    FString ReceivedString = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));
                    UE_LOG(LogTemp, Log, TEXT("UProcSocketObject::Received Data: %s"), *ReceivedString);

                    // ť�� ������ �߰�
                    ReceivedDataQueue.Enqueue(ReceivedString);
                }
                else
                {
                    // �����͸� ��ٸ��� ª�� ��� �ð��� �߰��մϴ�.
                    FPlatformProcess::Sleep(0.01f);
                }
            }
        });
}