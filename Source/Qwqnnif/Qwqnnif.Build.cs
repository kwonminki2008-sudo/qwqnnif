using UnrealBuildTool;

public class Qwqnnif : ModuleRules
{
	public Qwqnnif(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"AIModule",
			"GameplayTasks",
			"NavigationSystem"
		});
	}
}
