using UnrealBuildTool;
using System.IO;
 
public class MMT : ModuleRules
{
	public MMT(ReadOnlyTargetRules Target) : base (Target)
    {
        PrivateIncludePaths.AddRange(new string[] { "MMT/Private" });
        PrivatePCHHeaderFile = "Private/MMTPluginPCH.h";

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