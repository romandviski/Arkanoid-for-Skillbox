// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Arkanoid : ModuleRules
{
	public Arkanoid(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG"
		});

		// Чиним инклюды из подпапок проекта
		PublicIncludePaths.AddRange(new string[]
		{
			"Arkanoid"
		});
	}
}
