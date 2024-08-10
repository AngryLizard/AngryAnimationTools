

using UnrealBuildTool;

public class AngryAnimationTools : ModuleRules
{
	public AngryAnimationTools(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp20;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
                "AnimGraphRuntime",
                "RigVM",
                "ControlRig",
                "AnimationCore",
				"AngryUtility"
			}
			);

    }
}
