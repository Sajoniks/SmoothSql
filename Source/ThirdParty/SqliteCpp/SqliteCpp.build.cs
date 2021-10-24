// Fill out your copyright notice in the Description page of Project Settings.

using System;
using System.IO;
using UnrealBuildTool;

public class SqliteCpp : ModuleRules
{
	public SqliteCpp(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			string PluginPath = ModuleDirectory;
			Console.WriteLine("SqliteCpp Path: " + PluginPath);
			
			PublicIncludePaths.Add(Path.Combine(PluginPath, "include"));
			
			// Add the import library
			PublicAdditionalLibraries.Add(Path.Combine(PluginPath, "x64","Release", "SQLiteCpp.lib"));

			// Delay-load the DLL, so we can load it from the right place first
			PublicDelayLoadDLLs.Add("SQLiteCpp.dll");

			// Ensure that the DLL is staged along with the executable
			RuntimeDependencies.Add(Path.Combine(PluginPath, "x64", "Release", "SQLiteCpp.dll"));
			
			
			
			PublicIncludePaths.Add(Path.Combine(PluginPath, "sqlite", "include"));
			
			 // Add the import library
			PublicAdditionalLibraries.Add(Path.Combine(PluginPath, "sqlite", "x64", "Release", "sqlite3.lib"));

			 // Delay-load the DLL, so we can load it from the right place first
			PublicDelayLoadDLLs.Add("sqlite3.dll");

			 // Ensure that the DLL is staged along with the executable
			RuntimeDependencies.Add(Path.Combine(PluginPath, "sqlite", "x64", "Release", "sqlite3.dll"));
		}
	}
}
