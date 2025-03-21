using UnrealBuildTool;

public class CH4_TEAM3_BANG : ModuleRules
{
	public CH4_TEAM3_BANG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"Slate",
			"SlateCore",
			"UMG",
			"OnlineSubsystem",
			"AdvancedSessions",
			"AdvancedSteamSessions",
			"OnlineSubsystemSteam"
		});
		
        PrivateIncludePaths.Add("CH4_TEAM3_BANG");
    }
}
