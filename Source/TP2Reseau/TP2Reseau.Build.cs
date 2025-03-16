// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TP2Reseau : ModuleRules
{
	public TP2Reseau(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "OnlineSubsystemSteam", "OnlineSubsystem" });
	}
}
