// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PFTemplate : ModuleRules
{
	public PFTemplate(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
        {
        	"Core", 
        	"CoreUObject", 
        	"Engine", 
        	"InputCore", 
        	"ProceduralMeshComponent",
        	"EnhancedInput",
        	"Niagara",
	        "UMG"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { 
			"UnrealEd",      // For FAssetToolsModule
			"AssetTools",    // For CreateUniqueAssetName
			"AnimationCore"  // For BoneContainers
		});
		
		PublicIncludePaths.AddRange(new string[]
		{
			"PFTemplate/FighterStates/Public",
			"PFTemplate/Hitboxes/Public",
			"PFTemplate/Character",
			"PFTemplate/FighterStates",
			"PFTemplate/Input",
			"PFTemplate/ScriptedFrameLogic",
			"PFTemplate/Animation",
			"PFTemplate/Gamemode",
			"PFTemplate/Stage",
			"PFTemplate/UI",
			"PFTemplate/Rollback"
		});
	}
}
