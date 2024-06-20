// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VICTIMS : ModuleRules
{
	public VICTIMS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

        PublicIncludePaths.AddRange(new string[] {
                "VICTIMS/Public",
                "VICTIMS/Public/Player",
                "VICTIMS/Public/System",
            });

        PrivateIncludePaths.AddRange(new string[] {
                "VICTIMS/Private",
                "VICTIMS/Private/Player",
                "VICTIMS/Private/System",                
            });
    }
}
