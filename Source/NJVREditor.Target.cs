// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class NJVREditorTarget : TargetRules
{
	public NJVREditorTarget(TargetInfo Target): base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.Add("NJVR");
	}
}
