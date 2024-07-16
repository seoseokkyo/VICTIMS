// Fill out your copyright notice in the Description page of Project Settings.


#include "VictimsGameInstance.h"
#include "StateComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include <ServerProcSocket.h>
#include <InstantProcSocket.h>
#include <functional>
#include "VICTIMSCharacter.h"
#include "TestSaveGame.h"
#include "AVICTIMSPlayerController.h"

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
	// 클라이언트의 맵 이동 요청
	// 우선 런쳐 하나를 새로 연다 (같은 맵 요청이 들어와도 일단은 갈라놓자)
	// 파티가 있는지 검사를 한다
	// 클라이언트 소켓 연결을 대기한다.
	// Array에 쌓인 플레이어들의 상태를 소켓으로 쏜다
	// 싹 다 밀어넣는다.

	//일단 쳐내기
	if (InstantServerArray.Num() > 0)
	{
		return;
	}

	FInstantServerStruct newServerInst;
	newServerInst.serverNumber = InstantServerArray.Num() + 1;
	newServerInst.serverPortNumber = newServerInst.serverNumber + 8001;
	newServerInst.serverLevelName = NewLevelName;
	newServerInst.playerNum = 0;
	newServerInst.clients.Add(PlayerController);

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

	//// 일단 한놈 갑니다
	//PlayerController->ClientTravel(URL, type);

	// 여기서 클라이언트 소켓의 응답을 대기를 해야 하는데........ 흠
	FTimerHandle timer;
	clientTravelTimers.Add(timer);
	int currentIndex = clientTravelTimers.Num() - 1;

	InstantServerArray.Add(newServerInst);

	GetWorld()->GetTimerManager().SetTimer(clientTravelTimers[currentIndex], [&, currentIndex]() {

		TryToClientTravel(currentIndex);

		}, 1.0f, true);
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
			// 메인 서버 로직
			UE_LOG(LogTemp, Warning, TEXT("This is the Main Server"));

			mySocket = NewObject<UServerProcSocket>(GetWorld(), UServerProcSocket::StaticClass());

			InstantServerArray.Reset();
		}
		else if (serverType == "InstanceServer")
		{
			mySocket = NewObject<UInstantProcSocket>(GetWorld(), UInstantProcSocket::StaticClass());

			// 새로운 서버 인스턴스 로직
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
		else if (serverType == "InstanceServer")
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

void UVictimsGameInstance::TryToClientTravel(int waitIndex)
{
	if (serverType == "MainServer")
	{
		auto serverSocketCheck = Cast<UServerProcSocket>(mySocket);
		if (serverSocketCheck)
		{
			if (serverSocketCheck->ClientSockets.Num() > 0)
			{
				auto clientSocketCheck = serverSocketCheck->ClientSockets[waitIndex];

				if (clientSocketCheck)
				{
					if (clientSocketCheck->GetConnectionState() == SCS_Connected)
					{
						clientSocketCheck->GetPortNo();

						UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("clientSocketCheck:%d"), clientSocketCheck->GetPortNo()));

						//InstantServerArray[waitIndex].clients[waitIndex]->ClientTravel();
					}
				}
			}
		}
	}
}

void UVictimsGameInstance::ServerRPC_PrintServerType_Implementation()
{
	MultiRPC_PrintServerType(serverType);
}

void UVictimsGameInstance::MultiRPC_PrintServerType_Implementation(const FString& serverTypeString)
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("SERVER TYPE : %s"), *serverTypeString));
}

//====================================================================================================================
// Save

void UVictimsGameInstance::ShutDown()
{
	AVICTIMSPlayerController* PC = Cast<AVICTIMSPlayerController>(GetFirstLocalPlayerController());
	if (PC)
	{
		PC->SaveData(PC->PlayerID);
	}

	

	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [&](){
	
	Super::Shutdown();
	},0.5f, false);
}
