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

	// ULevelStreaming 객체 생성
	//ULevelStreaming* LevelStreaming = ULevelStreaming::NewLevelStreaming(LevelPackageName, LevelPackageName, FName(*LevelName), FTransform::Identity);

    int mapsNum = GetWorld()->GetStreamingLevels().Num();
    ULevelStreaming* LevelStreamingPersistent = NewObject<ULevelStreaming>(GetWorld(), ULevelStreaming::StaticClass(), FName(FString::Printf(TEXT("Map_%d"), mapsNum)));

    

    GetWorld()->AddStreamingLevel(LevelStreamingPersistent);
}

void UVictimsGameInstance::ConnectToServerAndMoveToNewLevel_Implementation(APlayerController* PlayerController, const FString& NewLevelName, ETravelType type)
{
    FString ServerAddress = "192.168.0.147"; // 서버 주소
    int32 ServerPort = 7777; // 서버 포트
    FString DefaultLevelName = "/Game/Maps/DefaultLevel"; // 서버의 기본 레벨

    // 서버에 접속
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

    // 기본 레벨에서 새로운 레벨로 이동
    if (PlayerController)
    {
		//ServerAddress.Append(NewLevelName);
        PlayerController->ClientTravel(NewLevelName, type);
        
		bCheck = PlayerController->HasAuthority();

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("PlayerController->HasAuthority : %s"), bCheck ? TEXT("TRUE") : TEXT("FALSE")));
    }
}

// 클라이언트에서 서브레벨을 로드하는 함수 예시
void UVictimsGameInstance::LoadSubLevel_Implementation(APlayerController* PlayerController, const FString& LevelName)
{
    // 월드 객체 가져오기
    UWorld* World = PlayerController->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to get world in LoadSubLevel"));
        return;
    }

    // 레벨 이름과 맵 경로 설정
    FString PackageName = "/Game/" + LevelName;
    FString PackagePath = FPackageName::GetLongPackagePath(PackageName);
    FString LevelPackageName = FPackageName::ObjectPathToPackageName(PackagePath);

    // ULevelStreaming 객체 생성
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

        // 클라이언트에게 새로운 레벨 로드 요청
        PlayerController->ClientTravel(LevelName, ETravelType::TRAVEL_Absolute);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create level streaming object for %s"), *LevelName);
    }
}