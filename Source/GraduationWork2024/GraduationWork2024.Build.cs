// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GraduationWork2024 : ModuleRules
{
	public GraduationWork2024(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput" });
    }
}
