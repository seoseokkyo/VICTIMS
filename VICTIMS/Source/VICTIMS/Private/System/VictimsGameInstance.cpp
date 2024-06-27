// Fill out your copyright notice in the Description page of Project Settings.


#include "VictimsGameInstance.h"
#include "StateComponent.h"
#include <Kismet/KismetSystemLibrary.h>

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

void UVictimsGameInstance::StreamingLevelTest()
{
	//FString PackageName = "/Game/Maps/" + LevelName;
	//FString PackagePath = FPackageName::GetLongPackagePath(PackageName);
	//FString LevelPackageName = FPackageName::ObjectPathToPackageName(PackagePath);

	// ULevelStreaming ��ü ����
	//ULevelStreaming* LevelStreaming = ULevelStreaming::NewLevelStreaming(LevelPackageName, LevelPackageName, FName(*LevelName), FTransform::Identity);

    int mapsNum = GetWorld()->GetStreamingLevels().Num();
    ULevelStreaming* LevelStreamingPersistent = NewObject<ULevelStreaming>(GetWorld(), ULevelStreaming::StaticClass(), FName(FString::Printf(TEXT("Map_%d"), mapsNum)));

    

    GetWorld()->AddStreamingLevel(LevelStreamingPersistent);
}

void UVictimsGameInstance::ConnectToServerAndMoveToNewLevel_Implementation(APlayerController* PlayerController, const FString& NewLevelName, ETravelType type)
{
    FString ServerAddress = "192.168.0.147"; // ���� �ּ�
    int32 ServerPort = 7777; // ���� ��Ʈ
    FString DefaultLevelName = "/Game/Maps/DefaultLevel"; // ������ �⺻ ����

    // ������ ����
    //FString URL = FString::Printf(TEXT("%s:%d"), *ServerAddress, ServerPort);

	bool bCheck = PlayerController->HasAuthority();
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("PlayerController->HasAuthority : %s"), bCheck ? TEXT("TRUE") : TEXT("FALSE")));

    FString URL = FString::Printf(TEXT("192.168.0.147/Game/%s"), *NewLevelName);
    //if (PlayerController)
    //{
    //    PlayerController->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
	//
	//	GetWorld()->ServerTravel();
    //}

    // �⺻ �������� ���ο� ������ �̵�
    if (PlayerController)
    {
		//ServerAddress.Append(NewLevelName);
        PlayerController->ClientTravel(NewLevelName, type);
        
		bCheck = PlayerController->HasAuthority();

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("PlayerController->HasAuthority : %s"), bCheck ? TEXT("TRUE") : TEXT("FALSE")));
    }
}

// Ŭ���̾�Ʈ���� ���극���� �ε��ϴ� �Լ� ����
void UVictimsGameInstance::LoadSubLevel_Implementation(APlayerController* PlayerController, const FString& LevelName)
{
    // ���� ��ü ��������
    UWorld* World = PlayerController->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to get world in LoadSubLevel"));
        return;
    }

    // ���� �̸��� �� ��� ����
    FString PackageName = "/Game/" + LevelName;
    FString PackagePath = FPackageName::GetLongPackagePath(PackageName);
    FString LevelPackageName = FPackageName::ObjectPathToPackageName(PackagePath);

    // ULevelStreaming ��ü ����
    ULevelStreaming* LevelStreaming = NewObject<ULevelStreaming>(World, ULevelStreaming::StaticClass(), FName(*LevelName));
    if (LevelStreaming)
    {
        LevelStreaming->SetWorldAssetByPackageName(*LevelPackageName);
        LevelStreaming->SetShouldBeLoaded(true);
        LevelStreaming->SetShouldBeVisible(true);
        //World->StreamingLevels.Add(LevelStreaming);
        auto arrayTest = World->GetStreamingLevels();
        arrayTest.Add(LevelStreaming);
        World->SetStreamingLevels(arrayTest);

        // Ŭ���̾�Ʈ���� ���ο� ���� �ε� ��û
        PlayerController->ClientTravel(LevelName, ETravelType::TRAVEL_Absolute);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create level streaming object for %s"), *LevelName);
    }
}