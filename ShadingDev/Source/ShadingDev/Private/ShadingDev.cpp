// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShadingDev.h"
#include "Interfaces/IPluginManager.h"
#include "EngineModule.h"
#include "SimpleShaders.h"

#define LOCTEXT_NAMESPACE "FShadingDevModule"

void FShadingDevModule::StartupModule()
{
	FString ShaderPath = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("ShadingDev"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/ShadingDev"), ShaderPath);
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
#if 0
	// Spawn Renderer in Render thread
	ENQUEUE_RENDER_COMMAND(CreateRadiationRenderer)
	(
		[this](FRHICommandListImmediate& RHICmdList)
		{
			m_renderer = MakeShared<FSimpleRenderer>();
		}
	);
#endif
}

void FShadingDevModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FShadingDevModule, ShadingDev)