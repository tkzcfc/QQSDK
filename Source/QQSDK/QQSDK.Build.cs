// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class QQSDK : ModuleRules
{
	public QQSDK(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
                "Engine",
                "Projects",
                "ApplicationCore"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            // Add third framework
            PublicAdditionalFrameworks.Add(
                new UEBuildFramework(
                   "TencentOpenAPI",// Framework name
                   "../../lib/IOS/TencentOpenAPI.embeddedframework.zip"
                )
            );

            PublicFrameworks.AddRange(
                new string[]
                {
                    "Security",
                    "SystemConfiguration",
                    "CoreGraphics",
                    "CoreTelephony",
                }
            );

            PublicAdditionalLibraries.Add("iconv");
            PublicAdditionalLibraries.Add("sqlite3");
            PublicAdditionalLibraries.Add("stdc++");
            PublicAdditionalLibraries.Add("z");
        }

        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "Launch"
            });
        }

        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "QQSDK_APL.xml"));
        }
    }
}
