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

		PrivateDependencyModuleNames.AddRange(new string[] 
		{ 
    		"AnimationCore"  // Usually safe for runtime
		});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
			PrivateDependencyModuleNames.Add("AssetTools"); 
		}
		
		PublicIncludePaths.AddRange(new string[]
		{
			"PFTemplate/FighterStates/Public",
			"PFTemplate/Combat/Public",
			"PFTemplate/Character",
			"PFTemplate/FighterStates",
			"PFTemplate/Input",
			"PFTemplate/ScriptedFrameLogic",
			"PFTemplate/Animation/Public",
			"PFTemplate/Gamemode",
			"PFTemplate/Stage",
			"PFTemplate/UI",
			"PFTemplate/Rollback",
			"PFTemplate/Movement",
			"PFTemplate/Combat/Hitboxes/Public",
			"PFTemplate/Combat/Projectiles",
			"PFTemplate/Effects/Public",
			"PFTemplate/Combat/Grabs",
			"PFTemplate/Rollback/CollisionSystem/Public",
			"PFTemplate/Bakery/Public"
		});
	}
}
