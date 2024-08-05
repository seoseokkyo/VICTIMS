// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VICTIMS : ModuleRules
{
	public VICTIMS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule", "NavigationSystem", "PhysicsCore", "UMG", "Slate", "SlateCore", "HTTP", "Json", "JsonUtilities", "Networking", "Sockets", "Media", "MediaAssets" });

        PublicIncludePaths.AddRange(new string[] {
                "VICTIMS/Public",
                "VICTIMS/Public/Player",
                "VICTIMS/Public/System",
                "VICTIMS/Public/Item",
                "VICTIMS/Public/Character",
                "VICTIMS/Public/Combat",
                "VICTIMS/Public/AnimNotify",
                "VICTIMS/Public/Inventory",
                "VICTIMS/Public/Youtube",
                "VICTIMS/Public/Blackjack",
                "VICTIMS/Public/Build",
                "VICTIMS/Public/UI",
                "VICTIMS/Public/ChatSystem",

            });

        PrivateIncludePaths.AddRange(new string[] {
                "VICTIMS/Private",
                "VICTIMS/Private/Player",
                "VICTIMS/Private/System",
                "VICTIMS/Private/Item",
                "VICTIMS/Private/Character",
                "VICTIMS/Private/Combat",
                "VICTIMS/Private/AnimNotify",
                "VICTIMS/Private/Inventory",
                "VICTIMS/Private/Youtube",
                "VICTIMS/Private/Blackjack",
                "VICTIMS/Private/Build",
                "VICTIMS/Private/UI",
                "VICTIMS/Private/ChatSystem",
            });
    }
}