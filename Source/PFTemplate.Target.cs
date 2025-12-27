// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PFTemplateTarget : TargetRules // Ensure this class name matches the filename
{
	public PFTemplateTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		
		// The new UE 5.6+ way to handle the warning conflict:
		CppCompileWarningSettings.UndefinedIdentifierWarningLevel = WarningLevel.Error;

		ExtraModuleNames.Add("PFTemplate");
	}
}
