// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ShooterMulti : ModuleRules
{
	public ShooterMulti(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Slate", "SlateCore", "AIModule", "GameplayTasks", "OnlineSubsystem", "OnlineSubsystemUtils", "MoviePlayer", "Networking", "Sockets" });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
        /*if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
		{
				if (UEBuildConfiguration.bCompileSteamOSS == true)
				{
					DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
				}
		}*/
    }
}
