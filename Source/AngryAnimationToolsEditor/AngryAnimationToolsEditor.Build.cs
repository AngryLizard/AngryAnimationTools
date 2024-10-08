

using UnrealBuildTool;

public class AngryAnimationToolsEditor : ModuleRules
{
	public AngryAnimationToolsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp20;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "ControlRig",
                "ControlRigEditor",
                "Sequencer",
                "LevelSequence",
                "MovieScene",
                "MovieSceneTracks",
                "LevelSequenceEditor",
                "EditorStyle",
                "AnimationEditor",
                "UnrealEd"
            }
			);
	}
}
