// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TestGame4 : ModuleRules
{
	public TestGame4(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"Niagara",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"TestGame4",
			"TestGame4/Variant_Strategy",
			"TestGame4/Variant_Strategy/UI",
			"TestGame4/Variant_TwinStick",
			"TestGame4/Variant_TwinStick/AI",
			"TestGame4/Variant_TwinStick/Gameplay",
			"TestGame4/Variant_TwinStick/UI",
			"TestGame4/Variant_SpaceStation",
			"TestGame4/Variant_SpaceStation/Gameplay",
			"TestGame4/Variant_SpaceStation/AI",
			"TestGame4/Variant_SpaceStation/Components",
			"TestGame4/Variant_SpaceStation/UI",
			"TestGame4/Variant_OrbitalSalvage",
			"TestGame4/Variant_OrbitalSalvage/Gameplay",
			"TestGame4/Variant_OrbitalSalvage/Components",
			"TestGame4/Variant_OrbitalSalvage/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
