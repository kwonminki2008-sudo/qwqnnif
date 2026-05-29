using UnrealBuildTool;
using System.Collections.Generic;

public class QwqnnifTarget : TargetRules
{
	public QwqnnifTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Qwqnnif");
	}
}
