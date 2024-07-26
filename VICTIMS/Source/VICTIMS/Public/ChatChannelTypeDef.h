#pragma once


//채팅 채널 이늄
UENUM(BlueprintType)
enum class EChatChannelType : uint8
{
	None = 0 UMETA(DisplayName = None),
	Global = 1 UMETA(DisplayName = Global),
	Friend = 2 UMETA(DisplayName = Friend),
	Party = 3 UMETA(DisplayName = Party),
	Private = 4 UMETA(DisplayName = Private),
	Announcement = 5 UMETA(DisplayName = Announcement),
	Game = 6 UMETA(DisplayName = Game),
	SystemLog = 7 UMETA(DisplayName = SystemLog)
};
