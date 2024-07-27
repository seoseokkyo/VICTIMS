#pragma once


//채팅 채널 이늄
UENUM(BlueprintType)
enum class EChatChannelType : uint8
{
	None = 0 UMETA(DisplayName = None),
	Global = 1 UMETA(DisplayName = Global),
	//Friend = 2 UMETA(DisplayName = Friend),
	Party = 2 UMETA(DisplayName = Party),
	Private = 3 UMETA(DisplayName = Private),
	Announcement = 4 UMETA(DisplayName = Announcement),
	Game = 5 UMETA(DisplayName = Game),
	SystemLog = 6 UMETA(DisplayName = SystemLog)
};
