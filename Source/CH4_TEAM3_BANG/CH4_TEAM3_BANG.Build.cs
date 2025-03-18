// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CH4_TEAM3_BANG : ModuleRules
{
	public CH4_TEAM3_BANG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
