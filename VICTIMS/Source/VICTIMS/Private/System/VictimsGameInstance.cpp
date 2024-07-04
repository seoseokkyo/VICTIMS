// Fill out your copyright notice in the Description page of Project Settings.


#include "VictimsGameInstance.h"
#include "StateComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include <ServerProcSocket.h>
#include <InstantProcSocket.h>

FCharacterStat UVictimsGameInstance::GetCharacterDataTable(const FString& rowName)
{
	if (dt_characerStatDataTable != nullptr)
	{
		FString errorText;
		FCharacterStat* characerStatInfo = dt_characerStatDataTable->FindRow<FCharacterStat>(FName(rowName), errorText);

		if (characerStatInfo != nullptr)
		{
			return *characerStatInfo;
		}
		else
		{
			return FCharacterStat();
		}
	}

	return FCharacterStat();
}

void UVictimsGameInstance::ConnectToServerAndMoveToNewLevel_Implementation(APlayerController* PlayerController, const FString& NewLevelName, ETravelType type)
{
	// Ŭ���̾�Ʈ�� �� �̵� ��û
	// �켱 ���� �ϳ��� ���� ���� (���� �� ��û�� ���͵� �ϴ��� �������)
	// ��Ƽ�� �ִ��� �˻縦 �Ѵ�
	// Ŭ���̾�Ʈ ���� ������ ����Ѵ�.
	// Array�� ���� �÷��̾���� ���¸� �������� ���
	// �� �� �о�ִ´�.

	//�ϴ� �ĳ���
	if (InstantServerArray.Num() > 0)
	{
		return;
	}

	FInstantServerStruct newServerInst;
	newServerInst.serverNumber = InstantServerArray.Num() + 1;
	newServerInst.serverPortNumber = newServerInst.serverNumber + 8001;
	newServerInst.serverLevelName = NewLevelName;
	newServerInst.playerNum = 0;

	FString ExecutablePath = FPaths::Combine(FPaths::ProjectDir(), TEXT("/Build/WindowsServer/VICTIMSServer.exe"));

	FString strMapName = FString::Printf(TEXT("\"%s?listen\" "), *newServerInst.serverLevelName);
	FString Params = FString::Printf(TEXT("%s-serverType=InstanceServer -PORT=%d -log"), *strMapName, newServerInst.serverPortNumber);

	FProcHandle ProcessHandle = FPlatformProcess::CreateProc(*ExecutablePath, *Params, true, false, false, nullptr, 0, nullptr, nullptr);
	if (ProcessHandle.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Launched new process with parameters: %s"), *Params);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to launch new process with parameters: %s"), *Params);
	}

	//while (1)
	//{
	//	if (Cast<UServerProcSocket>(mySocket)->bConnection == true)
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("Cast<UServerProcSocket>(mySocket)->bConnection == true"));
	//		break;
	//	}
	//}

	//FString URL = FString::Printf(TEXT("192.168.0.147:%d"), newServerInst.serverPortNumber);

	//// �ϴ� �ѳ� ���ϴ�
	//PlayerController->ClientTravel(URL, type);

	InstantServerArray.Add(newServerInst);
}

void UVictimsGameInstance::OnStart()
{
	Super::OnStart();

	FString ServerTypeCheck;
	if (FParse::Value(FCommandLine::Get(), TEXT("serverType="), ServerTypeCheck))
	{
		serverType = ServerTypeCheck;

		if (serverType == "MainServer")
		{
			// ���� ���� ����
			UE_LOG(LogTemp, Warning, TEXT("This is the Main Server"));

			mySocket = NewObject<UServerProcSocket>(GetWorld(), UServerProcSocket::StaticClass());

			InstantServerArray.Reset();
		}
		else if (serverType == "InstanceServer")
		{
			mySocket = NewObject<UInstantProcSocket>(GetWorld(), UInstantProcSocket::StaticClass());

			// ���ο� ���� �ν��Ͻ� ����
			UE_LOG(LogTemp, Warning, TEXT("This is an Instance Server"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerType not specified in command line arguments"));
	}

	if (FParse::Value(FCommandLine::Get(), TEXT("PORT="), serverPort))
	{
		if (serverType == "MainServer")
		{
			Cast<UServerProcSocket>(mySocket)->InitializeSocket();
		}
		else
		{
			int32 portNum = FCString::Atoi(*serverPort);
			int32 instNum = portNum - 8001;

			Cast<UInstantProcSocket>(mySocket)->SetInstantProcNumber(instNum);
			Cast<UInstantProcSocket>(mySocket)->SetInstantProcPortNum(portNum);

			Cast<UInstantProcSocket>(mySocket)->InitializeSocket();

			UE_LOG(LogTemp, Warning, TEXT("This is an Instance Server Numer : %d"), instNum);
			UE_LOG(LogTemp, Warning, TEXT("This is an Instance Server Port : %d"), portNum);
		}

		UE_LOG(LogTemp, Warning, TEXT("PORT : %s"), *serverPort);
	}
}

void UVictimsGameInstance::ServerRPC_PrintServerType_Implementation()
{
	//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("SERVER TYPE : %s"), *serverType));
	MultiRPC_PrintServerType(serverType);
}

void UVictimsGameInstance::MultiRPC_PrintServerType_Implementation(const FString& serverTypeString)
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("SERVER TYPE : %s"), *serverTypeString));

	if (InstantServerArray.Num() > 0)
	{
		FString strMapName = FString::Printf(TEXT("\"%s?listen\" "), *InstantServerArray[0].serverLevelName);
		FString Params = FString::Printf(TEXT("%s-serverType=InstanceServer -PORT=%d -log"), *strMapName, InstantServerArray[0].serverPortNumber);
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("TEST : %s"), *FPaths::Combine(FPaths::ProjectDir(), TEXT("/Build/WindowsServer/VICTIMSServer.exe"))));
	}
}