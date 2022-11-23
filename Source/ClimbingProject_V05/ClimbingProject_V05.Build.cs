// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ClimbingProject_V05 : ModuleRules
{
	public ClimbingProject_V05(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
