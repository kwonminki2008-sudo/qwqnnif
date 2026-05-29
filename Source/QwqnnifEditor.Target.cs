using UnrealBuildTool;
using System.Collections.Generic;

public class QwqnnifEditorTarget : TargetRules
{
	public QwqnnifEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Qwqnnif");
	}
}
