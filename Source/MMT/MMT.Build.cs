using UnrealBuildTool;
using System.IO;
 
public class MMT : ModuleRules
{
    public MMT(TargetInfo Target)
    {
        PrivateIncludePaths.AddRange(new string[] { "MMT/Private" });
        PublicIncludePaths.AddRange(new string[] { "MMT/Public" });
 
        PublicDependencyModuleNames.AddRange(
		new string[] { 
				"Core", 
				"CoreUObject", 
				"Engine", 
				"InputCore",			
				"RHI",
				"RenderCore",
				"UMG",
				"Slate",
				"SlateCore",
				"PhysX",
                "APEX"
            }
		);
    }
}