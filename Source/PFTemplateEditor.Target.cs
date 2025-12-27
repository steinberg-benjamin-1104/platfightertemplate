// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PFTemplateEditorTarget : TargetRules // Ensure "Editor" is in the class name here
{
	public PFTemplateEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		
		// Match the same warning setting as the Engine
		CppCompileWarningSettings.UndefinedIdentifierWarningLevel = WarningLevel.Error;

		ExtraModuleNames.Add("PFTemplate");
	}
}